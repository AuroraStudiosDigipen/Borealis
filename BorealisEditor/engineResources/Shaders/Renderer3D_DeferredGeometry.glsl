#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

uniform mat4 u_ModelTransform;

layout(std140) uniform Camera
{
    mat4 u_ViewProjection;
    vec4 CameraPos;
	mat4 u_invViewProj;
};

uniform int u_EntityID;

//Animations
uniform bool u_HasAnimation;
uniform int  u_AnimationIndex;

const int MAX_BONES = 128;
const int MAX_BONE_INFLUENCE = 4;
const int MAX_ANIMATIONS = 6;

struct Animation
{
    mat4 FinalBonesMatrices[MAX_BONES];
};

layout(std140) uniform AnimationUBO
{
    Animation animations[MAX_ANIMATIONS];
};

//out variables
out vec2 v_TexCoord;
out vec3 v_FragPos;
out vec3 v_Normal;
out vec3 v_Tangent;
out vec3 v_Bitangent;
flat out int v_EntityID;

void GeometryPass()
{
	v_TexCoord = vec2(a_TexCoord.x, a_TexCoord.y);

	v_FragPos = vec3(u_ModelTransform * vec4(a_Position, 1.0));
	
	mat3 normalMatrix = transpose(inverse(mat3(u_ModelTransform)));
    vec3 N = normalize(normalMatrix * a_Normal);
    vec3 T = normalize(normalMatrix * a_Tangent);
    T = normalize(T - dot(T, N) * N); // Gram-Schmidt orthogonalization
    vec3 B = cross(N, T);

    v_Normal = N;
    v_Tangent = T;
    v_Bitangent = B;

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
            vec4 localPosition = animations[u_AnimationIndex].FinalBonesMatrices[boneIds[i]] * vec4(a_Position,1.0f);
            TotalPosition += localPosition * weights[i];
        }

		gl_Position = u_ViewProjection * u_ModelTransform * TotalPosition;	
		//v_LightPos = u_LightViewProjection * u_ModelTransform * TotalPosition;
	}
	else
	{
		gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);	
		//v_LightPos = u_LightViewProjection * vec4(v_FragPos, 1.0);	
	}
	
	v_EntityID = u_EntityID;
}

void main()
{
	GeometryPass();
}

#type fragment
#version 410 core

// Geometry Pass Fragment Shader (GBuffer Writing)
layout(location = 0) out vec4 gAlbedo;       		// Albedo + Alpha
layout(location = 1) out int entityIDs;
layout(location = 2) out vec4 gNormal;       		// Normal
layout(location = 3) out vec4 gEmissive;     		// Emissive
layout(location = 4) out vec4 gRoughnessMetallic;   // Roughness + Metallic

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_Tangent;
in vec3 v_Bitangent;
flat in int v_EntityID;

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
    bool nonRepeatingTiles;

    bool hasAlbedoMap;
    bool hasEmissionMap;
    bool hasNormalMap;
    bool hasMetallicMap;
};

layout(std140) uniform MaterialUBO
{
    MaterialUBOData materials[128];
};

uniform int materialIndex;

uniform sampler2D albedoMap;
uniform sampler2D emissionMap;
uniform sampler2D metallicMap;
uniform sampler2D normalMap;

uniform bool u_lightPass;

vec2 GetTexCoord() 
{
	return v_TexCoord * materials[materialIndex].tiling + materials[materialIndex].offset;
}

vec4 GetAlbedoColor()
{
	vec4 albedoColor = vec4(0.f);
	if(materials[materialIndex].hasAlbedoMap) 
	{
		albedoColor = texture(albedoMap, GetTexCoord()) + materials[materialIndex].albedoColor;
	}
	else
	{
		albedoColor = materials[materialIndex].albedoColor;
	} 
	return albedoColor;
}

vec3 GetEmission()
{
    return  materials[materialIndex].hasEmissionMap ? texture(emissionMap, GetTexCoord()).rgb + materials[materialIndex].emissionColor.rgb: materials[materialIndex].emissionColor.rgb;
}

float GetMetallic() 
{
	return materials[materialIndex].hasMetallicMap ? texture(metallicMap, GetTexCoord()).r : materials[materialIndex].metallic;
}

float GetRoughness()
{
	return 1.f - materials[materialIndex].smoothness;
}

void GeometryPass()
{
	vec4 albedo = GetAlbedoColor();
    gAlbedo = albedo;

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
    gNormal = vec4(normal,1.f);

	gEmissive = vec4(GetEmission(), 1.f);

	gRoughnessMetallic = vec4(GetRoughness(), GetMetallic(), 1.f,1.f);
	entityIDs = v_EntityID;
}

void main()
{
    GeometryPass();
}