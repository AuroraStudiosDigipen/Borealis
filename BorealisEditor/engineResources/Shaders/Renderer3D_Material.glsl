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

	gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);	
	v_LightPos = u_LightViewProjection * vec4(v_FragPos, 1.0);	
	v_EntityID = u_EntityID;
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
	vec3 spotLightDirection;

	//float range;
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
//in vec3 v_Normal;

uniform mat4 u_ViewProjection;
uniform vec3 u_ViewPos;
uniform Material u_Material;
const int MAX_LIGHTS = 20;
uniform Light u_Lights[20];
uniform int u_LightsCount;
			

uniform sampler2D u_ShadowMap;
uniform bool shadowPass = false;

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

        float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess * u_Material.smoothness);

        vec3 diffuse = light.diffuse * diff * (1.0 - metallic);
        vec3 specular = light.specular * spec * GetSpecular() * metallic; 

        color = ambient + diffuse + specular + emission;
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

        float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess * u_Material.smoothness);

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
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance); 

	// ambient
	vec3 ambient = light.ambient * GetAlbedoColor().rgb;
	vec3 color = ambient;
	float metallic = GetMetallic();
	vec3 emission = GetEmission();

	vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);

    if (diff > 0.0)
    {
        float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess * u_Material.smoothness);

        float theta = dot(lightDir, normalize(-light.spotLightDirection)); 

        float epsilon = light.innerOuterAngle.x - light.innerOuterAngle.y;
        float intensity = clamp((theta - light.innerOuterAngle.y) / epsilon, 0.0, 1.0); 

        vec3 diffuse = light.diffuse * diff * (1.0 - metallic);
        vec3 specular = light.specular * spec * GetSpecular() * metallic;

		ambient *= intensity * attenuation;
		diffuse *= intensity * attenuation;
		specular *= intensity * attenuation;

		//temp
		float shadowFactor = GetShadowFactorSpotLight();

        color = ambient + shadowFactor * (diffuse + specular + emission);
    }

	return color;
}

float GetShadowFactorSpotLight()
{
	vec3 projCoord = v_LightPos.xyz / v_LightPos.w;
	vec2 UVCoord;
	UVCoord.x = 0.5 * projCoord.x + 0.5;
	UVCoord.y = 0.5 * projCoord.y + 0.5;
	float z = 0.5 * projCoord.z + 0.5;

	float depth = texture(u_ShadowMap, UVCoord).x;

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

void Render3DPass()
{
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

	// vec4 color;
	// if (u_Light.type == 0) 
	// {
	// 	color = vec4(ComputeSpotLight(u_Light, normal, viewDir), GetAlbedoColor().a);
	// }
	// else if (u_Light.type == 1)
	// {
	// 	color = vec4(ComputeDirectionalLight(u_Light, normal, viewDir), GetAlbedoColor().a);
	// }
	// else if (u_Light.type == 2)
	// {
	// 	color = vec4(ComputePointLight(u_Light, normal, viewDir), GetAlbedoColor().a);
	// }

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
