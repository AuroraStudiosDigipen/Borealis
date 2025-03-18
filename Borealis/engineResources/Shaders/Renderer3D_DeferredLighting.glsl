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

vec3 GetWorldPosition(vec2 texCoord, float depth)
{
    vec4 ndcPos = vec4(texCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);  // NDC coordinates
    vec4 worldPos = u_invViewProj * ndcPos;  // Transform to world space
    worldPos /= worldPos.w;  // Perspective divide
    return worldPos.xyz;     // Return the world-space position
}

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

// vec3 ComputeSpotLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float smoothness, vec3 specular)
// {
// 	vec3 lightDir = normalize(light.position - fragPos);

// 	float distance = length(light.position - fragPos);
//     float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);

// 	vec3 ambient = light.ambient * albedo;
// 	vec3 color = ambient;

// 	vec3 halfwayDir = normalize(lightDir + viewDir);

//     float diff = max(dot(normal, lightDir), 0.0);

// 	if (diff > 0.0)
//     {
//         float spec = pow(max(dot(normal, halfwayDir), 0.0), /* "u_Material.shininess *" */ smoothness);

//         float theta = dot(lightDir, normalize(-light.direction)); 
//         float epsilon = light.innerOuterAngle.x - light.innerOuterAngle.y;
//         float intensity = clamp((theta - light.innerOuterAngle.y) / epsilon, 0.0, 1.0); 

//         vec3 diffuse = light.diffuse * diff * (1.0 - metallic);
//         vec3 specVec = light.specular * spec * specular * metallic;

// 		ambient *= intensity * attenuation;
// 		diffuse *= intensity * attenuation;
// 		specVec *= intensity * attenuation;
//         color = ambient + diffuse + specVec;// + emission;
//     }

// 	return color;
// }

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

    //float shadowFactor = GetCascadeShadowFactor(lightDir, normal);

    vec3 color = ComputeLight(albedo, roughness, metallic, light.diffuse, lightDir, viewDir, normal);

    // if(u_HasShadow)
    // {
    //     color *= shadowFactor;
    // }
    return color;
}

// vec3 ComputePointLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo,float metallic, float smoothness, vec3 specular)
// {
// 	vec3 lightDir = normalize(light.position - fragPos);

// 	float distance = length(light.position - fragPos);
//     float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);

// 	vec3 ambient = light.ambient * albedo;
// 	vec3 color = ambient;

// 	float diff = max(dot(normal, lightDir), 0.0);

// 	if (diff > 0.0) 
// 	{
//         vec3 halfwayDir = normalize(lightDir + viewDir);

//         float spec = pow(max(dot(normal, halfwayDir), 0.0), /* "u_Material.shininess *" */ smoothness);

//         vec3 diffuse = light.diffuse * diff * attenuation * (1.0 - metallic);
//         vec3 specular = light.specular * spec * specular * attenuation * metallic;

//         color += diffuse + specular; /*+ emission*/
//     }

// 	return color;
// }

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
			//finalColor += ComputeSpotLight(u_Lights[i], fragPos, normal, viewDir, albedo.rgb, metallic, smoothness, specular);
		}
        else if (u_Lights[i].type == 1) // Directional Light
        {
            finalColor += ComputeDirectionalLight(u_Lights[i], fragPos, normal, viewDir, albedo.rgb, metallic, roughness);
        }
		else if (u_Lights[i].type == 2) // Point Light
        {
            //finalColor += ComputePointLight(u_Lights[i], fragPos, normal, viewDir, albedo.rgb, metallic, smoothness, specular);
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