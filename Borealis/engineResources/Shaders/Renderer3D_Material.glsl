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
uniform mat4 u_FinalBonesMatrices[MAX_BONES]; //move to uniform buffer objects

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
	v_TexCoord = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y); //flip the texture //switch to compressonator

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

struct Material { //move to uniform buffer object
	sampler2D albedoMap;
	sampler2D specularMap;
	sampler2D metallicMap;
	sampler2D normalMap;
	sampler2D heightMap;
	sampler2D occlusionMap;
	sampler2D detailMaskMap;
	sampler2D emissionMap;

	vec4 albedoColor;    
	vec4 specularColor;   
	vec4 emissionColor;     

	vec2 tiling;
	vec2 offset;
	float smoothness;
	float shininess;
	float metallic;

	bool hasAlbedoMap;
    bool hasSpecularMap;
    bool hasNormalMap;
	bool hasMetallicMap;
    bool hasEmissionMap;
	bool hasHeightMap;
};

// layout(std140) uniform MaterialUBO
// {
//     vec4 albedoColor;
//     vec4 specularColor;
//     vec4 emissionColor;

//     vec2 tiling;
//     vec2 offset;

//     float smoothness;
//     float shininess;
//     float metallic;
//     float padding1;

// 	bool hasAlbedoMap;
//     bool hasSpecularMap;
//     bool hasNormalMap;
// 	bool hasMetallicMap;
// };

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
    float padding1;

    bool hasAlbedoMap;
    bool hasSpecularMap;
    bool hasNormalMap;
    bool hasMetallicMap;
};

layout(std140) uniform MaterialUBO
{
    // Define an array of material data
    MaterialUBOData materials[128];
};

struct Light //move to uniform buffer object
{
	int type; // 0 = Spotlight, 1 = Directional , 2 = Point
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
	vec2 innerOuterAngle;

	float linear;
	float quadratic;

	bool castShadow;
};

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal; 
flat in int v_EntityID;
in vec3 v_Tangent;
in vec3 v_Bitangent;
in vec4 v_LightPos;

//uniform mat4 u_ViewProjection;
uniform mat4 u_View;
uniform vec3 u_ViewPos;
//uniform Material u_Material;
const int MAX_LIGHTS = 20;
uniform Light u_Lights[20];
uniform int u_LightsCount = 0;
			

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

vec2 GetTexCoord() 
{
	return v_TexCoord * materials[materialIndex].tiling + materials[materialIndex].offset;
}

vec4 GetAlbedoColor()
{
	vec4 albedoColor = materials[materialIndex].hasAlbedoMap ? texture(albedoMap, GetTexCoord()) : materials[materialIndex].albedoColor;
	if (materials[materialIndex].hasAlbedoMap) {
		albedoColor = mix(materials[materialIndex].albedoColor, albedoColor, 0.8);
	}

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

vec3 GetEmission()
{
	return vec3(0.f);//materials[materialIndex].hasEmissionMap ? texture(materials[materialIndex].emissionMap, GetTexCoord()).rgb : materials[materialIndex].emissionColor.rgb;
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
	vec3 lightDir = normalize(-light.direction);
    vec3 ambient = light.ambient * GetAlbedoColor().rgb;

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 color = ambient;
	float metallic = GetMetallic();
	vec3 emission = GetEmission();

    if (diff > 0.0) 
	{
        vec3 halfwayDir = normalize(lightDir + viewDir);

        float spec = pow(max(dot(normal, halfwayDir), 0.0), materials[materialIndex].shininess * materials[materialIndex].smoothness);

        vec3 diffuse = light.diffuse * diff * (1.0 - metallic);
        vec3 specular = light.specular * spec * GetSpecular() * metallic; 

		//temp
		float shadowFactor = GetCascadeShadowFactor(lightDir, normal);

        if(u_HasShadow)
		{
			color += shadowFactor * (diffuse + specular + emission);
		}
		else
		{
			color += (diffuse + specular + emission);
		}
    }
	return color;
}

vec3 ComputePointLight(Light light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - v_FragPos);

	float distance = length(light.position - v_FragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance); 

	// ambient
	vec3 ambient = light.ambient * GetAlbedoColor().rgb;
	vec3 color = ambient;
	float metallic = GetMetallic();
	vec3 emission = GetEmission();

	float diff = max(dot(normal, lightDir), 0.0);

    if (diff > 0.0) 
	{
        vec3 halfwayDir = normalize(lightDir + viewDir);

        float spec = pow(max(dot(normal, halfwayDir), 0.0), materials[materialIndex].shininess * materials[materialIndex].smoothness);

        vec3 diffuse = light.diffuse * diff * attenuation * (1.0 - metallic);
        vec3 specular = light.specular * spec * GetSpecular() * attenuation * metallic;

        color = ambient + diffuse + specular + emission;
    }

	return color;
}

vec3 ComputeSpotLight(Light light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - v_FragPos);
	float distance = length(light.position - v_FragPos);

	// Distance attenuation
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance); 

	// Ambient lighting - reduce its intensity slightly to prevent excessive brightness in shadows
	vec3 ambient = light.ambient * GetAlbedoColor().rgb * 0.2; 
	vec3 color = ambient;
	float metallic = GetMetallic();
	vec3 emission = GetEmission();

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float diff = max(dot(normal, lightDir), 0.0);

    if (diff > 0.0)
    {
        // Specular calculation
        float spec = pow(max(dot(normal, halfwayDir), 0.0), materials[materialIndex].shininess * materials[materialIndex].smoothness);

        // Spotlight intensity based on angle
        float theta = dot(lightDir, normalize(-light.direction)); 
        float epsilon = light.innerOuterAngle.x - light.innerOuterAngle.y;
        float intensity = clamp((theta - light.innerOuterAngle.y) / epsilon, 0.0, 1.0); 

        // Diffuse and specular terms
        vec3 diffuse = light.diffuse * diff * (1.0 - metallic);
        vec3 specular = light.specular * spec * GetSpecular() * metallic;

        // Apply spotlight intensity and distance attenuation to diffuse and specular only
        diffuse *= intensity * attenuation;
        specular *= intensity * attenuation;

		// Apply shadow factor to diffuse, specular, and emission
		float shadowFactor = GetShadowFactor(lightDir, normal);
		if(u_HasShadow)
		{
			color += shadowFactor * (diffuse + specular + emission);
		}
		else
		{
			color += (diffuse + specular + emission);
		}
        
    }

	return color;
}

void Render3DPass()
{
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);

	//mat3 TBN = mat3(vec3(1.f), vec3(1.f), v_Normal);
	mat3 TBN = mat3(v_Tangent, v_Bitangent, v_Normal);
	vec3 normal;
    if (materials[materialIndex].hasNormalMap) 
    {
        // Sample normal map in tangent space
        vec3 tangentNormal = texture(normalMap, GetTexCoord()).rgb;
        tangentNormal = tangentNormal * 2.0 - 1.0;  // Convert from [0, 1] to [-1, 1]
        // Transform to world space
        normal = normalize(TBN * tangentNormal);
    }
    else
    {
        normal = normalize(v_Normal);
    }

	vec4 color = vec4(0.0);  // Initialize the final color to zero

	if(u_LightsCount > 0)
	{
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
	}
	else
	{
		color = GetAlbedoColor();
	}

	// Apply the alpha channel from the albedo color
	color.a = GetAlbedoColor().a;

	fragColor = color;
	
	entityIDs = v_EntityID;
}

void main() 
{
	if(!shadowPass)
	{
		Render3DPass();
	}
}
