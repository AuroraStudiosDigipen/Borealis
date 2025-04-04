#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;  // Vertex position

uniform mat4 u_ViewProjection;

out vec3 v_TexCoord;

void main()
{
    v_TexCoord = a_Position;
    vec4 pos = u_ViewProjection * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;
    //gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 410 core	

in vec3 v_TexCoord;

uniform samplerCube u_Skybox;

out vec4 FragColor;

uniform bool Red;

void main()
{
    if(Red)
    {
        FragColor = vec4(1.f, 0.f, 0.f, 1.f);
    }
    else

   { FragColor = texture(u_Skybox, v_TexCoord);}
}