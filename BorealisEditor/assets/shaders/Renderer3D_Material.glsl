#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;
layout(location = 5) in ivec4 a_BoneIds;
layout(location = 6) in vec3 a_Weights;

uniform mat4 u_ModelTransform;
uniform mat4 u_ViewProjection;
uniform int u_EntityID;

out vec2 v_TexCoord;
out vec3 v_FragPos;
out vec3 v_Tangent;
out vec3 v_Bitangent;
out vec3 v_Normal;
flat out int v_EntityID;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
	//v_TexCoord = a_TexCoord;
	v_TexCoord = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y);

	v_FragPos = vec3(u_ModelTransform * vec4(a_Position, 1.0));
	//v_Normal = mat3(transpose(inverse(u_ModelTransform))) * a_Normal;
	
	mat3 normalMatrix = transpose(inverse(mat3(u_ModelTransform)));
    vec3 N = normalize(normalMatrix * a_Normal);
    vec3 T = normalize(normalMatrix * a_Tangent);
    T = normalize(T - dot(T, N) * N); // Gram-Schmidt orthogonalization
    vec3 B = cross(N, T);

    v_Normal = N;
    v_Tangent = T;
    v_Bitangent = B;

	vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(pos,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
   }

	gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0) * totalPostion;	
	v_EntityID = u_EntityID;
}

#type fragment
#version 410 core			
//layout(location = 0) out vec4 color;
layout(location = 0) out vec4 fragColor;
layout(location = 1) out int entityIDs;

struct Material {
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

struct Light {
	int type; // 0 = Spotlight, 1 = Directional , 2 = Point
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;

	//float range;
	vec2 innerOuterAngle;

	float linear;
	float quadratic;
};

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal; 
flat in int v_EntityID;
in vec3 v_Tangent;
in vec3 v_Bitangent;
//in vec3 v_Normal;

uniform mat4 u_ViewProjection;
uniform vec3 u_ViewPos;
uniform Material u_Material;
uniform Light u_Light;
			
uniform sampler2D u_Texture;

vec2 GetTexCoord() 
{
	return v_TexCoord * u_Material.tiling + u_Material.offset;
}

vec4 GetAlbedoColor()
{
	vec4 albedoColor = u_Material.hasAlbedoMap ? texture(u_Material.albedoMap, GetTexCoord()) : u_Material.albedoColor;
	if (u_Material.hasAlbedoMap) {
		albedoColor = mix(u_Material.albedoColor, albedoColor, 0.8);
	}

	return albedoColor;
}

vec3 GetSpecular()
{
	return u_Material.hasSpecularMap ? texture(u_Material.specularMap, GetTexCoord()).rgb : u_Material.specularColor.rgb;
}

float GetMetallic() 
{
	return u_Material.hasMetallicMap ? texture(u_Material.metallicMap, GetTexCoord()).r : u_Material.metallic;
}

vec3 GetEmission()
{
	return u_Material.hasEmissionMap ? texture(u_Material.emissionMap, GetTexCoord()).rgb : u_Material.emissionColor.rgb;
}

vec3 ComputeDirectionalLight(vec3 normal, vec3 viewDir) 
{
	vec3 lightDir = normalize(-u_Light.direction);

    vec3 ambient = u_Light.ambient * GetAlbedoColor().rgb;
	vec3 color = ambient;
	float metallic = GetMetallic();
	vec3 emission = GetEmission();

	float diff = max(dot(normal, lightDir), 0.0);

    if (diff > 0.0) 
	{
        vec3 halfwayDir = normalize(lightDir + viewDir);

        float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess * u_Material.smoothness);

        vec3 diffuse = u_Light.diffuse * diff * (1.0 - metallic);
        vec3 specular = u_Light.specular * spec * GetSpecular() * metallic; 

        color = ambient + diffuse + specular + emission;
    }
	return color;
}

vec3 ComputePointLight(vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(u_Light.position - v_FragPos);

	float distance = length(u_Light.position - v_FragPos);
    float attenuation = 1.0 / (1.0 + u_Light.linear * distance + u_Light.quadratic * distance * distance); 

	// ambient
	vec3 ambient = u_Light.ambient * GetAlbedoColor().rgb;
	vec3 color = ambient;
	float metallic = GetMetallic();
	vec3 emission = GetEmission();

	float diff = max(dot(normal, lightDir), 0.0);

    if (diff > 0.0) 
	{
        vec3 halfwayDir = normalize(lightDir + viewDir);

        float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess * u_Material.smoothness);

        vec3 diffuse = u_Light.diffuse * diff * attenuation * (1.0 - metallic);
        vec3 specular = u_Light.specular * spec * GetSpecular() * attenuation * metallic;

        color = ambient + diffuse + specular + emission;
    }

	return color;
}

vec3 ComputeSpotLight(vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(u_Light.position - v_FragPos);

	float distance = length(u_Light.position - v_FragPos);
    float attenuation = 1.0 / (1.0 + u_Light.linear * distance + u_Light.quadratic * distance * distance); 

	// ambient
	vec3 ambient = u_Light.ambient * GetAlbedoColor().rgb;
	vec3 color = ambient;
	float metallic = GetMetallic();
	vec3 emission = GetEmission();

	vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);

    if (diff > 0.0)
    {
        float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess * u_Material.smoothness);

        float theta = dot(lightDir, normalize(-u_Light.direction)); 
        float epsilon = u_Light.innerOuterAngle.x - u_Light.innerOuterAngle.y;
        float intensity = clamp((theta - u_Light.innerOuterAngle.y) / epsilon, 0.0, 1.0); 

        vec3 diffuse = u_Light.diffuse * diff * (1.0 - metallic);
        vec3 specular = u_Light.specular * spec * GetSpecular() * metallic;

		ambient *= intensity * attenuation;
		diffuse *= intensity * attenuation;
		specular *= intensity * attenuation;
        color = ambient + diffuse + specular + emission;
    }

	return color;
}

void main() {
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);

	mat3 TBN = mat3(v_Tangent, v_Bitangent, v_Normal);
	vec3 normal;
    if (u_Material.hasNormalMap) 
    {
        // Sample normal map in tangent space
        vec3 tangentNormal = texture(u_Material.normalMap, GetTexCoord()).rgb;
        tangentNormal = tangentNormal * 2.0 - 1.0;  // Convert from [0, 1] to [-1, 1]
        // Transform to world space
        normal = normalize(TBN * tangentNormal);
    }
    else
    {
        normal = normalize(v_Normal);
    }

	vec4 color;
	if (u_Light.type == 0) 
	{
		color = vec4(ComputeSpotLight(normal, viewDir), GetAlbedoColor().a);
	}
	else if (u_Light.type == 1)
	{
		color = vec4(ComputeDirectionalLight(normal, viewDir), GetAlbedoColor().a);
	}
	else if (u_Light.type == 2)
	{
		color = vec4(ComputePointLight(normal, viewDir), GetAlbedoColor().a);
	}

	fragColor = color;
	
	entityIDs = v_EntityID;
}
