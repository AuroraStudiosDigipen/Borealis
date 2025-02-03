#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position; //pack into more compressed formats, switch to Structs of arrays
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

//default variables
uniform mat4 u_ModelTransform;
//uniform mat4 u_ViewProjection;
layout(std140) uniform Camera
{
	mat4 u_ViewProjection;
};
uniform int u_EntityID;

//Animation variables
uniform bool u_HasAnimation;
const int MAX_BONES = 128;
const int MAX_BONE_INFLUENCE = 4;
//uniform mat4 u_FinalBonesMatrices[MAX_BONES]; //move to uniform buffer objects

layout(std140) uniform AnimationUBO
{
	mat4 u_FinalBonesMatrices[MAX_BONES];
};

//shadow pass variables
uniform bool shadowPass;
uniform mat4 u_LightViewProjection;

out vec2 v_TexCoord;
out vec3 v_FragPos;
out vec3 v_Tangent;
out vec3 v_Bitangent;
out vec3 v_Normal;
out vec4 v_LightPos;
flat out int v_EntityID;

void ShadowPass()
{
	mat4 MVP = u_LightViewProjection * u_ModelTransform;
	gl_Position = MVP * vec4(a_Position, 1.0);
}

void Render3DPass()
{
    v_TexCoord = vec2(a_TexCoord.x, 1 - a_TexCoord.y);
	v_FragPos = vec3(u_ModelTransform * vec4(a_Position, 1.0));
	
	mat3 normalMatrix = transpose(inverse(mat3(u_ModelTransform))); //calculate T and N in compiler
    vec3 N = normalize(normalMatrix * a_Normal);
    vec3 T = normalize(normalMatrix * a_Tangent);
    T = normalize(T - dot(T, N) * N); // Gram-Schmidt orthogonalization
    vec3 B = cross(N, T);

    if(u_HasAnimation)
    {	
		//animation
		vec4 TotalPosition = vec4(0.f);

		//vec3 weightedNormal = vec3(0.0);
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(boneIds[i] == -1) 
                continue;
            if(boneIds[i] >= MAX_BONES) 
            {
                TotalPosition = vec4(a_Position,1.0f);
                break;
            }
            vec4 localPosition = u_FinalBonesMatrices[boneIds[i]] * vec4(a_Position,1.0f);
            TotalPosition += localPosition * weights[i];
			
			//weightedNormal += weights[i] * mat3(u_FinalBonesMatrices[boneIds[i]]) * a_Normal;

			//Need to apply weightedTangent and BitTangent as well
        }
		//N = normalize(weightedNormal);

		gl_Position = u_ViewProjection * u_ModelTransform * TotalPosition;	
		v_LightPos = u_LightViewProjection * u_ModelTransform * TotalPosition;
	}
	else
	{
		gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);	
		v_LightPos = u_LightViewProjection * vec4(v_FragPos, 1.0);	
	}
	
	v_EntityID = u_EntityID;
	v_Normal = N;
    v_Tangent = T;
    v_Bitangent = B;
}

void main()
{
	if(shadowPass)
	{
		ShadowPass();
	}
	else
	{
		Render3DPass();
	}
}

#type fragment
#version 410 core			
//layout(location = 0) out vec4 color;
layout(location = 0) out vec4 fragColor;
layout(location = 1) out int entityIDs;

struct MaterialUBOData
{
    vec4 albedoColor;
    vec4 specularColor;
    vec4 emissionColor;

    vec2 tiling;
    vec2 offset;

    float smoothness;
    float shininess;
    float metallic;
    float roughness;

    bool hasAlbedoMap;
    bool hasSpecularMap;
    bool hasNormalMap;
    bool hasMetallicMap;
};

layout(std140) uniform MaterialUBO
{
    MaterialUBOData materials[128];
};

struct Light //move to uniform buffer object
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

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal; 
flat in int v_EntityID;
in vec3 v_Tangent;
in vec3 v_Bitangent;
in vec4 v_LightPos;

uniform mat4 u_View;
uniform vec3 u_ViewPos;
			

uniform sampler2D u_ShadowMap;
uniform sampler2DArray u_CascadeShadowMap;
uniform bool shadowPass = false;
uniform bool u_HasShadow = false;
uniform mat4 u_LightSpaceMatrices[4];
uniform float u_CascadePlaneDistances[4];
uniform int cascadeCount;

uniform int materialIndex;
uniform sampler2D albedoMap;
uniform sampler2D specularMap;
uniform sampler2D metallicMap;
uniform sampler2D normalMap;


const float PI = 3.14159265359;

vec2 GetTexCoord() 
{
	return v_TexCoord * materials[materialIndex].tiling + materials[materialIndex].offset;
}

vec4 GetAlbedoColor()
{
	vec4 albedoColor = materials[materialIndex].hasAlbedoMap ? texture(albedoMap, GetTexCoord()) : materials[materialIndex].albedoColor;
	return albedoColor;
}

vec3 GetSpecular()
{
	return materials[materialIndex].hasSpecularMap ? texture(specularMap, GetTexCoord()).rgb : materials[materialIndex].specularColor.rgb;
}

float GetMetallic() 
{
	return materials[materialIndex].hasMetallicMap ? texture(metallicMap, GetTexCoord()).r : materials[materialIndex].metallic;
}

float GetRoughness()
{
	return 1.f - materials[materialIndex].smoothness;
}

vec3 GetEmission()
{
	return vec3(0.f);//materials[materialIndex].hasEmissionMap ? texture(materials[materialIndex].emissionMap, GetTexCoord()).rgb : materials[materialIndex].emissionColor.rgb;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float GetShadowFactor(vec3 lightDir, vec3 normal)
{
	vec3 projCoord = v_LightPos.xyz / v_LightPos.w;
	vec2 UVCoord;
	UVCoord.x = 0.5 * projCoord.x + 0.5;
	UVCoord.y = 0.5 * projCoord.y + 0.5;
	float z = 0.5 * projCoord.z + 0.5;

	float depth = texture(u_ShadowMap, UVCoord).x;

	float diffuseFactor = dot(normal, -lightDir);
	float bias = 0.0025;

	if(depth + bias < z)
	{
		return 0.5;
	}
	else
	{
		return 1.f;
	}
}

float GetCascadeShadowFactor(vec3 lightDir, vec3 normal)
{
	vec4 fragPosViewSpace = u_View * vec4(v_FragPos, 1.0);
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

	vec4 LightPos = u_LightSpaceMatrices[layer] * vec4(v_FragPos, 1.0);

	vec3 projCoord = LightPos.xyz / LightPos.w;
	vec2 UVCoord;
	UVCoord.x = 0.5 * projCoord.x + 0.5;
	UVCoord.y = 0.5 * projCoord.y + 0.5;
	float z = 0.5 * projCoord.z + 0.5;

	float currentDepth = projCoord.z;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        return 0.0;
    }

	float depth = texture(u_CascadeShadowMap, vec3(UVCoord, layer)).x;

	float diffuseFactor = dot(normal, -lightDir);
	float bias = 0.0025;//mix(0.0025f, 0.00f, diffuseFactor);

	if(depth + bias < z)
	{
		return 0.5;
	}
	else
	{
		return 1.f;
	}
}

vec3 ComputeDirectionalLight(Light light, vec3 normal, vec3 viewDir) 
{
    vec3 lightDir = vec3(0.f);

    lightDir = normalize(-light.direction) + 1e-5;

    float metallic = GetMetallic();

    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 radiance = light.ambient * 10.f;

    float D = DistributionGGX(normal, halfwayDir, GetRoughness());
    float G = GeometrySmith(normal, viewDir, lightDir, GetRoughness());

    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, GetAlbedoColor().rgb, metallic);

    vec3 F = fresnelSchlick(max(dot(viewDir, halfwayDir), 0.0), F0);

    float NdotL = max(dot(normal, lightDir), 0.0);

    vec3 specular = (D * G * F) / (4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 1e-5);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    // Apply shadow factor
    float shadowFactor = GetCascadeShadowFactor(lightDir, normal);

    vec3 color = vec3(0.f);
    
    if(u_HasShadow)
    {
        color += shadowFactor * ((GetAlbedoColor().xyz / PI + specular) * radiance * NdotL);
    }
    else
    {
        color += ((kD * GetAlbedoColor().xyz / PI + specular) * radiance * NdotL);
    }

    return color;
}

vec3 ComputePointLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 Lo = vec3(0.f);

    float metallic = GetMetallic();

    vec3 lightDir = normalize(light.position - v_FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float distance = length(light.position - v_FragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance); 

    vec3 radiance = light.ambient * attenuation;

    float D = DistributionGGX(normal, halfwayDir, GetRoughness());
    float G = GeometrySmith(normal, viewDir, lightDir, GetRoughness());

    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, GetAlbedoColor().rgb, metallic);

    vec3 F = fresnelSchlick(max(dot(lightDir, halfwayDir), 0.0), F0);

    vec3 specular = (D * G * F) / (4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 1e-5);

    vec3 kS = F;
    vec3 kD = max(vec3(0.0), vec3(1.0) - kS) * (1.0 - metallic);
    kD *= 1.0 - metallic;
    float NdotL = max(dot(normal, lightDir), 0.0); 
    
    Lo = ( GetAlbedoColor().xyz / PI + specular) * radiance * NdotL;

    return Lo;
}

vec3 ComputeSpotLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - v_FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float distance = length(light.position - v_FragPos);
    
    // Angular attenuation (spotlight cone)
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerOuterAngle.x - light.innerOuterAngle.y;
    float intensity = clamp((theta - light.innerOuterAngle.y) / epsilon, 0.0, 1.0);
    intensity *= intensity; // Quadratic falloff
    
    // Distance attenuation
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);
    
    // Combined attenuation
    float totalAttenuation = attenuation * intensity;
    
    // Early exit if outside spotlight cone
    //if(totalAttenuation <= 0.0) return vec3(0.0);

    // PBR calculations
    float metallic = GetMetallic();
    float roughness = GetRoughness();
    roughness = roughness * roughness; // Square roughness

    // Radiance calculation
    vec3 radiance = light.diffuse * totalAttenuation; // Use diffuse color as light color

    // BRDF components
    float D = DistributionGGX(normal, halfwayDir, roughness);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, GetAlbedoColor().rgb, metallic);
    vec3 F = fresnelSchlick(max(dot(lightDir, halfwayDir), 0.0), F0);

    vec3 specular = (D * G * F) / (4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 1e-5);
    
    // Energy conservation
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
    
    float NdotL = max(dot(normal, lightDir), 0.0);

    // Final composition

    vec3 color = vec3(0.f);

    float shadowFactor = GetShadowFactor(lightDir, normal);
    if (u_HasShadow)
    {
        color += shadowFactor * ((kD * GetAlbedoColor().rgb / PI + specular) * radiance * NdotL);
    }
    else
    {
        color += (kD * GetAlbedoColor().rgb / PI + specular) * radiance * NdotL;
    }

    return color;
}

void Render3DPass()
{
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);

	mat3 TBN = mat3(v_Tangent, v_Bitangent, v_Normal);
	vec3 normal;
    if (materials[materialIndex].hasNormalMap) 
    {
        vec3 tangentNormal = texture(normalMap, GetTexCoord()).rgb;
        tangentNormal = tangentNormal * 2.0 - 1.0; 
        normal = normalize(TBN * tangentNormal);
    }
    else
    {
        normal = normalize(v_Normal);
    }

	vec4 color = vec4(0.0);  // Initialize the final color to zero

    for (int i = 0; i < u_LightsCount; ++i)
    {
        if (u_Lights[i].type == 0)  // Spot Light
        {
            color.rgb += ComputeSpotLight(u_Lights[i], normal, viewDir);
        }
        else if (u_Lights[i].type == 1)  // Directional Light
        {
            color.rgb += ComputeDirectionalLight(u_Lights[i], normal, viewDir);
        }
        else if (u_Lights[i].type == 2)  // Point Light
        {
            color.rgb += ComputePointLight(u_Lights[i], normal, viewDir);
        }
    }

	vec3 ambient = vec3(0.1f) * GetAlbedoColor().rgb;

	vec3 finalColor = color.rgb;

	finalColor = finalColor / (finalColor + vec3(1.0));
    finalColor = pow(finalColor, vec3(1.0/2.2)); 
	fragColor =  vec4(finalColor,GetAlbedoColor().a);
	
	entityIDs = v_EntityID;
}

void main() 
{
	if(!shadowPass)
	{
		Render3DPass();
	}
}
