#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoord;
void SSAOBlurPass()
{
	v_TexCoord = a_TexCoords;
    gl_Position = vec4(a_Position, 1.0);
}

void main()
{
	SSAOBlurPass();
}

#type fragment
#version 410 core

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D ssaoTexture;

void SSAOBlurPass()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    
    // Define a 5x5 Gaussian kernel (weights sum to 1.0)
    float kernel[25] = float[](
        0.003, 0.013, 0.022, 0.013, 0.003,
        0.013, 0.059, 0.097, 0.059, 0.013,
        0.022, 0.097, 0.159, 0.097, 0.022,
        0.013, 0.059, 0.097, 0.059, 0.013,
        0.003, 0.013, 0.022, 0.013, 0.003
    );

    float result = 0.0;
    int index = 0;
    // Loop over a 5x5 region
    int r = 5
    for (int x = -r; x <= r; ++x)
    {
        for (int y = -r; y <= r; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float sample = texture(ssaoTexture, v_TexCoord + offset).r;
            result += sample * kernel[index];
            index++;
        }
    }
    
    FragColor = vec4(vec3(result), 1.0);
}

void main()
{
	SSAOBlurPass();
}