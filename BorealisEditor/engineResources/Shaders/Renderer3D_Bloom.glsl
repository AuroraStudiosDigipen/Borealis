#type vertex
#version 410 core
			
layout(location = 0) in vec2 a_Position;  // Vertex position
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = vec4(a_Position, 0.f, 1.0);
}

#type fragment
#version 410 core	

in vec2 v_TexCoord;
out vec4 FragColor;

uniform int u_Step;
uniform sampler2D u_SceneTexture;
uniform sampler2D u_BloomTexture;
// uniform float u_Threshold;
// uniform float u_Knee;

// float u_SampleScale = 1.f;

layout(std140) uniform SceneRenderUBO
{
    float u_Threshold;
    float u_Knee;
    float u_SampleScale;
    float exposure;
};

uniform vec2 u_TexelSize;


void ThresholdFilterPass()
{
    // vec3 color = texture(u_SceneTexture, v_TexCoord).rgb;
    // float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    // color *= smoothstep(u_Threshold - 0.1, u_Threshold + 0.1, brightness); // Soft threshold
    // FragColor = vec4(color, 1.0);

    vec3 color = texture(u_SceneTexture, v_TexCoord).rgb;
    float brightness = max(max(color.r, color.g), color.b);
    // Smooth transition using a soft knee
    float soft = clamp((brightness - u_Threshold + u_Knee) / u_Knee, 0.0, 1.0);
    soft = soft * soft;
    // Only allow colors above threshold (with soft blending)
    float contribution = max(soft, step(u_Threshold, brightness));
    FragColor = vec4(color * contribution, 1.0);
}

void DownsamplePass() {
    vec2 offsets[4] = vec2[](
        vec2(-0.5, -0.5) * u_TexelSize,
        vec2(-0.5, 0.5) * u_TexelSize,
        vec2(0.5, -0.5) * u_TexelSize,
        vec2(0.5, 0.5) * u_TexelSize
    );
    vec3 sum = vec3(0.0);
    for (int i = 0; i < 4; ++i) {
        sum += texture(u_SceneTexture, v_TexCoord + offsets[i]).rgb;
    }
    // Average the samples
    FragColor = vec4(sum * 0.25, 1.0);
}

void UpsamplePass() {
    vec3 center   = texture(u_SceneTexture, v_TexCoord).rgb;
    vec3 right    = texture(u_SceneTexture, v_TexCoord + vec2(u_TexelSize.x, 0.0)).rgb;
    vec3 left     = texture(u_SceneTexture, v_TexCoord - vec2(u_TexelSize.x, 0.0)).rgb;
    vec3 up       = texture(u_SceneTexture, v_TexCoord + vec2(0.0, u_TexelSize.y)).rgb;
    vec3 down     = texture(u_SceneTexture, v_TexCoord - vec2(0.0, u_TexelSize.y)).rgb;
    
    vec3 tentFiltered = (center + right + left + up + down) / 5.0;
    
    vec3 previousBloom = texture(u_BloomTexture, v_TexCoord).rgb;
    vec3 combined = tentFiltered * u_SampleScale + previousBloom;
    
    FragColor = vec4(combined, 1.0);
}

void CompositePass() {
    vec3 scene = texture(u_SceneTexture, v_TexCoord).rgb;
    vec3 bloom = texture(u_BloomTexture, v_TexCoord).rgb;
    // Simple additive composite
    FragColor = vec4(scene + bloom, 1.0);
}

void main()
{
    if(u_Step == 0)
    {
        ThresholdFilterPass();
    }
    else if(u_Step == 1)
    {
        DownsamplePass();
    }
    else if(u_Step == 2)
    {
        UpsamplePass();
    }
    else if(u_Step == 3)
    {
        CompositePass();
    }
    else if(u_Step == 4)
    {
        FragColor = vec4(texture(u_SceneTexture, v_TexCoord).rgb, 1.0);
    }
    else
    {
        vec3 finalColor = texture(u_SceneTexture, v_TexCoord).rgb;
        finalColor = vec3(1.f) - exp(-finalColor * exposure);
        //finalColor = finalColor / (finalColor + vec3(1.0));
        finalColor = pow(finalColor, vec3(1.0/2.2)); 
        FragColor = vec4(finalColor, 1.0);
    }
}