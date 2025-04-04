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

uniform sampler2D u_Texture0;
uniform float u_Alpha;

void main()
{
    vec4 color = texture(u_Texture0, v_TexCoord);
    if (isnan(color.r) || isnan(color.g) || isnan(color.b) || isnan(color.a)) {
        color = vec4(0.0); // Replace NaN with black
    }

    if(color.a >= 0.5)
    {
        color.a = u_Alpha;
    }
    
    FragColor = color;
}