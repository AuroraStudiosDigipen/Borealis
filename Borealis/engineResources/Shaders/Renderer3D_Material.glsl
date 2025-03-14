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
    vec4 CameraPos;
};
uniform int u_EntityID;

//Animation variables
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
    v_TexCoord = vec2(a_TexCoord.x, a_TexCoord.y);
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
            vec4 localPosition = animations[u_AnimationIndex].FinalBonesMatrices[boneIds[i]] * vec4(a_Position,1.0f);
            TotalPosition += localPosition * weights[i];
        }

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
layout(location = 2) out float outRevealage;
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

layout(std140) uniform Camera
{
    mat4 u_ViewProjection;
    vec4 CameraPos;
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


uniform bool u_Transparent = false;

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal; 
flat in int v_EntityID;
in vec3 v_Tangent;
in vec3 v_Bitangent;
in vec4 v_LightPos;

uniform mat4 u_View;

uniform sampler2D u_ShadowMap;
uniform sampler2DArray u_CascadeShadowMap;
uniform sampler2DArray u_CascadeShadowMapDynamic;
uniform bool shadowPass = false;
uniform bool u_HasShadow = false;
uniform mat4 u_LightSpaceMatrices[4];
uniform float u_CascadePlaneDistances[4];
uniform int cascadeCount;

uniform samplerCube u_cubeMap;

uniform int materialIndex;
uniform sampler2D albedoMap;
uniform sampler2D emissionMap;
uniform sampler2D metallicMap;
uniform sampler2D normalMap;

layout(std140) uniform SceneRenderUBO
{
    float u_Threshold;
    float u_Knee;
    float u_SampleScale;
    float exposure;
};

const float airIOR   = 1.0;
const float glassIOR = 1.5;

const float PI = 3.14159265359;

float Round(float num){ return floor(num + .5); }
vec3 Round3(vec3 ivec){ return floor(ivec + vec3(0.5)); }
vec2 Rotate(vec2 UV, float amount)
{
	vec2 center = vec2(.5) * materials[materialIndex].tiling;
	UV -= center;	
	vec2 rot = vec2(cos(amount), sin(amount));
	return vec2((rot.x * UV.x) + (rot.y * UV.y), (rot.x * UV.y) - (rot.y * UV.x)) + center;
}
vec3 Hash2(vec2 UV)
{
	return fract(sin(vec3(
		dot(vec3(UV.x, UV.y, UV.x), vec3(127.09, 311.7, 74.69)), 
		dot(vec3(UV.y, UV.x, UV.x), vec3(269.5, 183.3, 246.1)), 
		dot(vec3(UV.x, UV.y, UV.y), vec3(113.5, 271.89, 124.59))
	)) * 43758.5453);
}
float Lerp(float val1, float val2, float amount)
{
	return ( val2 - val1 ) * amount;	
}
vec2 Translate(vec2 UV, vec2 amount){ return UV + amount; }
vec2 Scale(vec2 UV, vec2 amount){ return UV * amount; }
vec2 Transform(vec2 UV, float rotation, vec2 scale, vec2 translation)
{
	return Translate(Scale(Rotate(UV, rotation), scale), translation);
}	
vec2 RandomTransform(vec2 UV, vec2 seed)
{
	vec3 hash = Hash2(seed);
	float rot = mix(-3.1415, 3.1415, fract(hash.b*16.));
	float scl = mix(.8, 1.2, hash.b);
	return Transform(UV, rot, vec2(scl), hash.xy);
}


vec2 GetTexCoord() 
{
	return v_TexCoord * materials[materialIndex].tiling + materials[materialIndex].offset;
}

vec4 GetAlbedoColor()
{
	vec4 albedoColor = vec4(0.f);
    if(materials[materialIndex].nonRepeatingTiles)
    {
        vec2 base_uv = GetTexCoord();
        vec2 uv	= vec2(base_uv);
        float hex_size = materials[materialIndex].specularColor.x;
        uv = vec2(uv.x - ((.5/(1.732 / 2.))*uv.y), (1./(1.732 / 2.))*uv.y) / hex_size;
        
        vec2 coord	= floor(uv);	
        vec4 color	= vec4(coord.x, coord.y, 0., 1.);			
        color.rgb = ((vec3(color.r - color.g) + vec3(0, 1, 2)) * .3333333) + 5./3.;																
        color.rgb = Round3(fract(color.rgb));						
        
        vec4 refcol = vec4(fract(vec2(uv.x, uv.y)), 1, 1);
        refcol.rgb = vec3(refcol.g + refcol.r) - 1.;
        vec4 abscol = vec4(abs(refcol.rgb), 1);
        
        vec4 refswz = vec4(fract(vec2(uv.y, uv.x)), 1, 1);
        vec4 use_col = vec4(fract(vec2(uv.x, uv.y)), 1, 1);
        
        float flip_check = 0.;
        if ( ((refcol.r+refcol.g+refcol.b)/3.) > 0. ){
            use_col = vec4(1.-refswz.x, 1.-refswz.y, refswz.b, refswz.a);
            flip_check = 1.;
        }
        
        float sharpness = materials[materialIndex].specularColor.y;
        abscol.rgb = abs(vec3(abscol.r, use_col.r, use_col.g));
        use_col.rgb = vec3(
            pow(dot(abscol.rgb, vec3(color.z, color.x, color.y)), sharpness), 
            pow(dot(abscol.rgb, vec3(color.y, color.z, color.x)), sharpness), 
            pow(dot(abscol.rgb, color.rgb), sharpness)
        );

        float coldot = dot(use_col.rgb, vec3(1));
        use_col /= coldot;
        
        vec2 color_swiz1 = vec2(color.a, color.z);
        vec2 color_swiz2 = vec2(color.z, color.x);
        vec2 color_swiz3 = vec2(color.x, color.a);
        
        color.rgb *= flip_check;
        vec4 ruv1 = texture( albedoMap, RandomTransform(base_uv, color_swiz1 + vec2(color.r) + coord)) * vec4(vec3(use_col.r), 1);
        vec4 ruv2 = texture( albedoMap, RandomTransform(base_uv, color_swiz2 + vec2(color.g) + coord)) * vec4(vec3(use_col.g), 1);
        vec4 ruv3 = texture( albedoMap, RandomTransform(base_uv, color_swiz3 + vec2(color.b) + coord)) * vec4(vec3(use_col.b), 1);
        albedoColor = ruv1 + ruv2 + ruv3;
    }
    else
    {
        if(materials[materialIndex].hasAlbedoMap) 
        {
            albedoColor = texture(albedoMap, GetTexCoord()) + materials[materialIndex].albedoColor;
        }
        else
        {
            albedoColor = materials[materialIndex].albedoColor;
        } 
    }
	return albedoColor;
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
    return  materials[materialIndex].hasEmissionMap ? texture(emissionMap, GetTexCoord()).rgb + materials[materialIndex].emissionColor.rgb: materials[materialIndex].emissionColor.rgb;
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

    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        return 0.0;
    }

    float depth = texture(u_CascadeShadowMap, vec3(UVCoord, layer)).x;
    float depth2 = texture(u_CascadeShadowMapDynamic, vec3(UVCoord, layer)).x;

    float diffuseFactor = dot(normal, -lightDir);
    float bias = max(0.0005, 0.005 * (1.0 - diffuseFactor));

    if ((depth + bias < z) || (depth2 + bias < z))
    {
        return 0.5;
    }
    else
    {
        return 1.0;
    }
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

vec3 ComputeDirectionalLight(Light light, vec3 normal, vec3 viewDir) 
{
    vec3 lightDir = vec3(0.f);

    lightDir = normalize(-light.direction);

    float shadowFactor = GetCascadeShadowFactor(lightDir, normal);

    vec3 albedo = GetAlbedoColor().rgb;
    float roughness = GetRoughness();
    float metallic = GetMetallic();
    vec3 color = ComputeLight(albedo, roughness, metallic, light.diffuse, lightDir, viewDir, normal);

    if(u_HasShadow)
    {
        color *= shadowFactor;
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
	vec3 viewDir = normalize(CameraPos.rgb - v_FragPos);

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

	vec3 finalColor = color.rgb + GetEmission();
    //finalColor = vec3(1.f) - exp(-finalColor * exposure);
    // finalColor = finalColor / (finalColor + vec3(1.0));
    // finalColor = pow(finalColor, vec3(1.0/2.2)); 

    if(u_Transparent)
    {
        float weight = clamp(pow(min(1.0, GetAlbedoColor().a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
        fragColor = vec4(finalColor.rgb * GetAlbedoColor().a * weight, GetAlbedoColor().a * weight);
        outRevealage = GetAlbedoColor().a;
    }
    else
    {
	    fragColor =  vec4(finalColor,GetAlbedoColor().a);
    }

	entityIDs = v_EntityID;
}

void main() 
{
	if(!shadowPass)
	{
		Render3DPass();

        //vec4 color = vec4(1.f);

        //3 texture map paths
        // if(materials[materialIndex].hasAlbedoMap)
        // {
        //     color = texture(albedoMap, GetTexCoord());
        // }

        // if(materials[materialIndex].hasMetallicMap && materials[materialIndex].hasSpecularMap)
        // {
        //     vec4 tile2 = texture(specularMap, GetTexCoord());
        //     vec4 path = texture(metallicMap, v_TexCoord);

        //     vec3 finalColor = vec3(0.f);

        //     //option 1
        //     float total = path.r + path.g;
        //     if (total > 0.0) {
        //         finalColor = (color.rgb * path.r + tile2.rgb * path.g) / total;
        //     } else {
        //         finalColor = (color.rgb + tile2.rgb) * 0.5; // Fallback for black path
        //     }

        //     //option 2
        //     finalColor = color.rgb * path.r + tile2.rgb * path.g;

        //     color = vec4(finalColor, 1.f);
        // }


        // fragColor = color;

        //non repeating tiles
        // vec2 base_uv = GetTexCoord();
        // vec2 uv	= vec2(base_uv);
        // float hex_size = u_Knee;
        // uv = vec2(uv.x - ((.5/(1.732 / 2.))*uv.y), (1./(1.732 / 2.))*uv.y) / hex_size;
        
        // vec2 coord	= floor(uv);	
        // vec4 color	= vec4(coord.x, coord.y, 0., 1.);			
        // color.rgb = ((vec3(color.r - color.g) + vec3(0, 1, 2)) * .3333333) + 5./3.;																
        // color.rgb = Round3(fract(color.rgb));						
        
        // vec4 refcol = vec4(fract(vec2(uv.x, uv.y)), 1, 1);
        // refcol.rgb = vec3(refcol.g + refcol.r) - 1.;
        // vec4 abscol = vec4(abs(refcol.rgb), 1);
        
        // vec4 refswz = vec4(fract(vec2(uv.y, uv.x)), 1, 1);
        // vec4 use_col = vec4(fract(vec2(uv.x, uv.y)), 1, 1);
        
        // float flip_check = 0.;
        // if ( ((refcol.r+refcol.g+refcol.b)/3.) > 0. ){
        //     use_col = vec4(1.-refswz.x, 1.-refswz.y, refswz.b, refswz.a);
        //     flip_check = 1.;
        // }
        
        // float sharpness = u_Threshold;
        // abscol.rgb = abs(vec3(abscol.r, use_col.r, use_col.g));
        // use_col.rgb = vec3(
        //     pow(dot(abscol.rgb, vec3(color.z, color.x, color.y)), sharpness), 
        //     pow(dot(abscol.rgb, vec3(color.y, color.z, color.x)), sharpness), 
        //     pow(dot(abscol.rgb, color.rgb), sharpness)
        // );
    
        // float coldot = dot(use_col.rgb, vec3(1));
        // use_col /= coldot;
        
        // vec2 color_swiz1 = vec2(color.a, color.z);
        // vec2 color_swiz2 = vec2(color.z, color.x);
        // vec2 color_swiz3 = vec2(color.x, color.a);
        
        // color.rgb *= flip_check;
        // vec4 ruv1 = texture( albedoMap, RandomTransform(base_uv, color_swiz1 + vec2(color.r) + coord)) * vec4(vec3(use_col.r), 1);
        // vec4 ruv2 = texture( albedoMap, RandomTransform(base_uv, color_swiz2 + vec2(color.g) + coord)) * vec4(vec3(use_col.g), 1);
        // vec4 ruv3 = texture( albedoMap, RandomTransform(base_uv, color_swiz3 + vec2(color.b) + coord)) * vec4(vec3(use_col.b), 1);
        // vec4 rout = ruv1 + ruv2 + ruv3;

        // fragColor = rout;
	}
}