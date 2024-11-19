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

void main()
{
    FragColor = texture(u_Texture0, v_TexCoord);
}