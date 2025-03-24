#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoord;
void SSAOPass()
{
	v_TexCoord = a_TexCoords;
    gl_Position = vec4(a_Position, 1.0);
}

void main()
{
	SSAOPass();
}

#type fragment
#version 410 core

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D gNormalTexture;
uniform sampler2D gDepthTexture;
uniform sampler2D noiseTexture;

layout(std140) uniform Camera
{
    mat4 u_ViewProjection;
    vec4 CameraPos;
	mat4 u_invViewProj;
	mat4 u_projection;
    float screenHeight;
    float screenWidth;
    //add screen width and height
};

layout(std140) uniform NoiseSample
{
    vec4 samples[64]; //vec3 + padding
};

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

vec2 noiseScale = vec2(screenWidth/4.0, screenHeight/4.0); 

vec3 GetWorldPosition(vec2 texCoord, float depth)
{
    vec4 ndcPos = vec4(texCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);  // NDC coordinates
    vec4 worldPos = u_invViewProj * ndcPos;  // Transform to world space
    worldPos /= worldPos.w;  // Perspective divide
    return worldPos.xyz;     // Return the world-space position
}

void SSAOPass()
{
    vec3 normal = texture(gNormalTexture, v_TexCoord).rgb;

	float depth = texture(gDepthTexture, v_TexCoord).r;
	vec3 fragPos = GetWorldPosition(v_TexCoord, depth);

    vec3 randomVec = normalize(texture(noiseTexture, v_TexCoord * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;

    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i].xyz; // from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = u_projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = texture(gDepthTexture, offset.xy).r;
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    //FragColor = occlusion;
    FragColor = vec4(fragPos, 1.f);
}

void main()
{
	SSAOPass();
}