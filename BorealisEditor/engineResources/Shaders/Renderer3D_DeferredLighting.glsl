#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

uniform mat4 u_ModelTransform;
uniform mat4 u_ViewProjection;
uniform int u_EntityID;
uniform bool u_lightPass;

out vec2 v_TexCoord;
out vec3 v_FragPos;
out vec3 v_Normal;
out vec3 v_Tangent;
out vec3 v_Bitangent;
flat out int v_EntityID;

void GeometryPass()
{
	v_TexCoord = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y);

	v_FragPos = vec3(u_ModelTransform * vec4(a_Position, 1.0));
	
	mat3 normalMatrix = transpose(inverse(mat3(u_ModelTransform)));
    vec3 N = normalize(normalMatrix * a_Normal);
    vec3 T = normalize(normalMatrix * a_Tangent);
    T = normalize(T - dot(T, N) * N); // Gram-Schmidt orthogonalization
    vec3 B = cross(N, T);

    v_Normal = N;
    v_Tangent = T;
    v_Bitangent = B;

	gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);	
	v_EntityID = u_EntityID;
}

void LightPass()
{
	v_TexCoord = a_Position.xy * 0.5 + 0.5; // Convert from [-1, 1] to [0, 1]
    gl_Position = vec4(a_Position, 1.0);
}

void main()
{
	if(u_lightPass)
	{
		LightPass();
	}
	else
	{
		GeometryPass();
	}
}

#type fragment
#version 410 core

// Geometry Pass Fragment Shader (GBuffer Writing)
layout(location = 0) out vec4 gAlbedo;       // Albedo + Alpha
layout(location = 1) out int entityIDs;
layout(location = 2) out vec4 gNormal;       // Normal (in world space or view space) + smoothness
layout(location = 3) out vec4 gSpecular;     // Specular color (can also store shininess here) + metallic
layout(location = 4) out vec3 gPosition;     // World-space position (optional if reconstructing from depth)
//layout(location = 5) out float gMetallic;    // Metallic factor

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;
in vec3 v_Tangent;
in vec3 v_Bitangent;
flat in int v_EntityID;

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

uniform Material u_Material;

uniform sampler2D lAlbedo;
uniform isampler2D lEntityID;
uniform sampler2D lNormal;
uniform sampler2D lSpecular;
uniform sampler2D lPosition;  // Optional if using world-space positions
//uniform sampler2D lMetallic;
//uniform sampler2D lRoughness;

uniform vec3 u_ViewPos;
const int MAX_LIGHTS = 20;
uniform Light u_Lights[MAX_LIGHTS];
uniform int u_LightsCount;

uniform bool u_lightPass;


vec2 GetTexCoord() 
{
    return v_TexCoord * u_Material.tiling + u_Material.offset;
}

vec3 ComputeSpotLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float smoothness, vec3 specular)
{
	vec3 lightDir = normalize(light.position - fragPos);

	float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);

	vec3 ambient = light.ambient * albedo;
	vec3 color = ambient;

	vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);

	if (diff > 0.0)
    {
        float spec = pow(max(dot(normal, halfwayDir), 0.0), /* "u_Material.shininess *" */ smoothness);

        float theta = dot(lightDir, normalize(-light.direction)); 
        float epsilon = light.innerOuterAngle.x - light.innerOuterAngle.y;
        float intensity = clamp((theta - light.innerOuterAngle.y) / epsilon, 0.0, 1.0); 

        vec3 diffuse = light.diffuse * diff * (1.0 - metallic);
        vec3 specVec = light.specular * spec * specular * metallic;

		ambient *= intensity * attenuation;
		diffuse *= intensity * attenuation;
		specVec *= intensity * attenuation;
        color = ambient + diffuse + specVec;// + emission;
    }

	return color;
}

vec3 ComputeDirectionalLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float smoothness, vec3 specular) 
{
    vec3 lightDir = normalize(-light.direction);
    vec3 ambient = light.ambient * albedo;

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 color = ambient;
    if (diff > 0.0) 
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), /*"shininess *" */smoothness); //add shininess

        vec3 diffuse = light.diffuse * diff * (1.0 - metallic);
        vec3 specularComponent = light.specular * spec * specular * metallic;

        color += diffuse + specularComponent; /*+ emission*/
    }
    return color;
}

vec3 ComputePointLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo,float metallic, float smoothness, vec3 specular)
{
	vec3 lightDir = normalize(light.position - fragPos);

	float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);

	vec3 ambient = light.ambient * albedo;
	vec3 color = ambient;

	float diff = max(dot(normal, lightDir), 0.0);

	if (diff > 0.0) 
	{
        vec3 halfwayDir = normalize(lightDir + viewDir);

        float spec = pow(max(dot(normal, halfwayDir), 0.0), /* "u_Material.shininess *" */ smoothness);

        vec3 diffuse = light.diffuse * diff * attenuation * (1.0 - metallic);
        vec3 specular = light.specular * spec * specular * attenuation * metallic;

        color += diffuse + specular; /*+ emission*/
    }

	return color;
}

void GeometryPass()
{
	vec4 albedo = u_Material.hasAlbedoMap ? texture(u_Material.albedoMap, GetTexCoord()) : u_Material.albedoColor;
    gAlbedo = albedo;

	mat3 TBN = mat3(v_Tangent, v_Bitangent, v_Normal);
    vec3 normal;
    if (u_Material.hasNormalMap) 
    {
        vec3 tangentNormal = texture(u_Material.normalMap, GetTexCoord()).rgb;
        tangentNormal = tangentNormal * 2.0 - 1.0;  // Convert from [0,1] to [-1,1]
        normal = normalize(TBN * tangentNormal);
    }
    else
    {
        normal = normalize(v_Normal);
    }
    gNormal = vec4(normal,u_Material.smoothness);

	vec3 specular = u_Material.hasSpecularMap ? texture(u_Material.specularMap, GetTexCoord()).rgb : u_Material.specularColor.rgb;

	float metallic = u_Material.hasMetallicMap ? texture(u_Material.metallicMap, GetTexCoord()).r : u_Material.metallic;
    gSpecular = vec4(specular, metallic);
	gPosition = v_FragPos;
	entityIDs = v_EntityID;
}

void LightPass()
{
	vec4 albedo = texture(lAlbedo, v_TexCoord);
    vec3 normal = normalize(texture(lNormal, v_TexCoord).rgb);
    vec3 specular = texture(lSpecular, v_TexCoord).rgb;
    vec3 fragPos = texture(lPosition, v_TexCoord).rgb;  // If storing world-space positions
    float metallic = texture(lSpecular, v_TexCoord).a;
    float smoothness = texture(lNormal, v_TexCoord).a;

	vec3 viewDir = normalize(u_ViewPos - fragPos);
    vec3 finalColor = vec3(0.0);

	for (int i = 0; i < u_LightsCount; ++i)
    {
		if(u_Lights[i].type == 0) // Spot light
		{
			finalColor += ComputeSpotLight(u_Lights[i], fragPos, normal, viewDir, albedo.rgb, metallic, smoothness, specular);
		}
        else if (u_Lights[i].type == 1) // Directional Light
        {
            finalColor += ComputeDirectionalLight(u_Lights[i], fragPos, normal, viewDir, albedo.rgb, metallic, smoothness, specular);
        }
		else if (u_Lights[i].type == 2) // Point Light
        {
            finalColor += ComputePointLight(u_Lights[i], fragPos, normal, viewDir, albedo.rgb, metallic, smoothness, specular);
        }
    }

    gAlbedo = vec4(finalColor, albedo.a);
	entityIDs = texture(lEntityID, v_TexCoord).r;
}

void main()
{
	if(u_lightPass)
	{
		LightPass();
	}
	else
	{
		GeometryPass();
	}

	//temp
	if(u_Material.hasEmissionMap) {u_Material.emissionColor; u_Material.shininess;}
}