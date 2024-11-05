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
	v_EntityID = u_EntityID;
}

void LightingPass()
{
	v_TexCoord = a_Position.xy * 0.5 + 0.5; // Convert from [-1, 1] to [0, 1]
    gl_Position = vec4(a_Position, 1.0);
}

void main()
{
	GeometryPass();
}

#type fragment
#version 410 core

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

layout(location = 0) out vec4 gAlbedo;       // Albedo + Alpha
layout(location = 1) out vec3 gNormal;       // Normal (in world space or view space)
layout(location = 2) out vec3 gSpecular;     // Specular color (can also store shininess here)
layout(location = 3) out vec3 gPosition;     // World-space position (optional if reconstructing from depth)
layout(location = 4) out float gMetallic;    // Metallic factor
layout(location = 5) out float gRoughness;   // Roughness factor (optional)

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;
in vec3 v_Tangent;
in vec3 v_Bitangent;
flat in int v_EntityID;

uniform mat4 u_ViewProjection;
uniform vec3 u_ViewPos;
uniform Material u_Material;
const int MAX_LIGHTS = 20;
uniform Light u_Lights[20];
uniform int u_LightsCount;
			
uniform sampler2D u_Texture;

void GeometryPass()
{

}

void LightingPass()
{

}

void main()
{
	GeometryPass();
}