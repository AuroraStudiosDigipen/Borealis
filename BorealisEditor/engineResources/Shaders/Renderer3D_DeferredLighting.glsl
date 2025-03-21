#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;

out vec2 v_TexCoord;
void LightPass()
{
	v_TexCoord = a_Position.xy * 0.5 + 0.5; // Convert from [-1, 1] to [0, 1]
    gl_Position = vec4(a_Position, 1.0);
}

void main()
{
	LightPass();
}

#type fragment
#version 410 core

// Geometry Pass Fragment Shader (GBuffer Writing)
layout(location = 0) out vec4 gAlbedo;       		// Albedo + Alpha
layout(location = 1) out int entityIDs;

in vec2 v_TexCoord;

uniform sampler2D lAlbedo;
uniform isampler2D lEntityID;
uniform sampler2D lNormal;
uniform sampler2D lEmissive;
uniform sampler2D lRoughnessMetallic;
uniform sampler2D lDepthBuffer;

layout(std140) uniform Camera
{
    mat4 u_ViewProjection;
    vec4 CameraPos;
	mat4 u_invViewProj;
};

//Lights UBO
struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
	vec2 innerOuterAngle;
	float linear;
	float quadratic;

	int type; // 0 = Spotlight, 1 = Directional , 2 = Point
	bool castShadow;

	vec2 padding;
};

const int MAX_LIGHTS = 32;
layout(std140) uniform LightsUBO
{
	Light u_Lights[MAX_LIGHTS];
	int u_LightsCount;
};

const float PI = 3.14159265359;

//Shadow Variables
uniform mat4 u_View;

uniform sampler2D u_ShadowMap;
uniform sampler2DArray u_CascadeShadowMap;
uniform sampler2DArray u_CascadeShadowMapDynamic;
uniform bool shadowPass = false;
uniform bool u_HasShadow = false;
uniform mat4 u_LightSpaceMatrices[4];
uniform float u_CascadePlaneDistances[4];
uniform int cascadeCount;

vec3 GetWorldPosition(vec2 texCoord, float depth)
{
    vec4 ndcPos = vec4(texCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);  // NDC coordinates
    vec4 worldPos = u_invViewProj * ndcPos;  // Transform to world space
    worldPos /= worldPos.w;  // Perspective divide
    return worldPos.xyz;     // Return the world-space position
}

//Shadow
float GetCascadeShadowFactor(vec3 lightDir, vec3 normal, vec3 fragPos)
{
    vec4 fragPosViewSpace = u_View * vec4(fragPos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue < u_CascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }

    vec4 LightPos = u_LightSpaceMatrices[layer] * vec4(fragPos, 1.0);

    vec3 projCoord = LightPos.xyz / LightPos.w;
    vec2 UVCoord;
    UVCoord.x = 0.5 * projCoord.x + 0.5;
    UVCoord.y = 0.5 * projCoord.y + 0.5;
    float z = 0.5 * projCoord.z + 0.5;

    float currentDepth = projCoord.z;

    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        return 0.0;
    }

    float depth = texture(u_CascadeShadowMap, vec3(UVCoord, layer)).x;
    float depth2 = texture(u_CascadeShadowMapDynamic, vec3(UVCoord, layer)).x;

    float diffuseFactor = dot(normal, -lightDir);
    float bias = 0.01f;//max(0.0005, 0.005 * (1.0 - diffuseFactor));

    if ((depth + bias < z) || (depth2 + bias < z))
    {
        return 0.5;
    }
    else
    {
        return 1.0;
    }
}

//PBR
float NewDistributionGGX(float NdotH, float a) 
{
    float a2 = a * a;
    float f = (NdotH * a2 - NdotH) * NdotH + 1.0;
    return a2 / (PI * f * f);
}
  
float NewGeometrySmith(float NdotV, float NdotL, float a) 
{
    float a2 = a * a;
    float GGXV = NdotL * sqrt((NdotV - NdotV * a2) * NdotV + a2);
    float GGXL = NdotV * sqrt((NdotL - NdotL * a2) * NdotL + a2);
    return 0.5 / (GGXV + GGXL);
}

vec3 NewFresnelSchlick(float LdotH, vec3 F0) 
{
    return F0 + (vec3(1.0) - F0) * pow(1.0 - LdotH, 5.0);
}

vec3 ComputeLight(vec3 albedo, float roughness,
    float metallic, vec3 lightDiffuse,
    vec3 lightDir,
    vec3 viewDir, vec3 normal) 
{
    vec3 halfVector = normalize(lightDir + viewDir);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    float a = max(roughness * roughness, 0.002025);

    float NdotV = max(dot(normal, viewDir), 1e-4);
    float NdotL = clamp(dot(normal, lightDir), 0.0, 1.0);
    float NdotH = clamp(dot(normal, halfVector), 0.0, 1.0);
    float LdotH = clamp(dot(lightDir, halfVector), 0.0, 1.0);

    float D = NewDistributionGGX(NdotH, a);
    vec3 F = NewFresnelSchlick(LdotH, F0);
    float V = NewGeometrySmith(NdotV, NdotL, a);

    vec3 specular = (D * V) * F;

    vec3 diffuseColor = (1.0 - metallic) * albedo;
    vec3 diffuse = diffuseColor / PI;

    return (diffuse * lightDiffuse + specular) * NdotL;
}

vec3 ComputeDirectionalLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness) 
{
	vec3 lightDir = vec3(0.f);

    lightDir = normalize(-light.direction);

    float shadowFactor = GetCascadeShadowFactor(lightDir, normal, fragPos);

    vec3 color = ComputeLight(albedo, roughness, metallic, light.diffuse, lightDir, viewDir, normal);

    if(u_HasShadow)
    {
        color *= shadowFactor;
    }
    return color;
}

vec3 ComputeSpotLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness)
{
	vec3 lightDir = normalize(light.position - fragPos);
    
	float distance = length(light.position - fragPos);

    // Angular attenuation (spotlight cone)
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerOuterAngle.x - light.innerOuterAngle.y;
    float intensity = clamp((theta - light.innerOuterAngle.y) / epsilon, 0.0, 1.0);
    intensity *= intensity; // Quadratic falloff
    
    // Distance attenuation
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);
    
    // Combined attenuation
    float totalAttenuation = attenuation * intensity;

    vec3 color = ComputeLight(albedo, roughness, metallic, light.diffuse, lightDir, viewDir, normal);

    color *= totalAttenuation;

	return color;
}

vec3 ComputePointLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness)
{
	vec3 lightDir = normalize(light.position - fragPos);

	float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);

	float diff = max(dot(normal, lightDir), 0.0);

    vec3 color = ComputeLight(albedo, roughness, metallic, light.diffuse, lightDir, viewDir, normal);

    color *= attenuation;

	return color;
}

void LightPass()
{
	vec4 albedo = texture(lAlbedo, v_TexCoord);
    vec3 normal = texture(lNormal, v_TexCoord).rgb;
    float metallic = texture(lRoughnessMetallic, v_TexCoord).g;
    float roughness = texture(lRoughnessMetallic, v_TexCoord).r;
	vec3 emission = texture(lEmissive, v_TexCoord).rgb;

	float depth = texture(lDepthBuffer, v_TexCoord).r;
	vec3 fragPos = GetWorldPosition(v_TexCoord, depth);

	vec3 viewDir = normalize(CameraPos.xyz - fragPos);
    vec3 finalColor = vec3(0.0);

	for (int i = 0; i < u_LightsCount; ++i)
    {
		if(u_Lights[i].type == 0) // Spot light
		{
			finalColor += ComputeSpotLight(u_Lights[i], fragPos, normal, viewDir, albedo.rgb, metallic, roughness);
		}
        else if (u_Lights[i].type == 1) // Directional Light
        {
            finalColor += ComputeDirectionalLight(u_Lights[i], fragPos, normal, viewDir, albedo.rgb, metallic, roughness);
        }
		else if (u_Lights[i].type == 2) // Point Light
        {
            finalColor += ComputePointLight(u_Lights[i], fragPos, normal, viewDir, albedo.rgb, metallic, roughness);
        }
    }

	finalColor += emission;

    gAlbedo = vec4(finalColor, albedo.a);
	entityIDs = texture(lEntityID, v_TexCoord).r;
}

void main()
{
	LightPass();
}