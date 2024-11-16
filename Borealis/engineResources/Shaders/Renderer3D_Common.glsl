#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;  // Vertex position
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_ModelTransform;

void MainPass()
{
    a_TexCoord;
    gl_Position = u_ViewProjection * u_ModelTransform * vec4(a_Position, 1.0);
}

void main()
{
    MainPass();
}

#type fragment
#version 410 core	

uniform vec4 u_Color; //uniform color for now
uniform bool u_Filled = false;
uniform bool u_HighlightPass = true;

out vec4 FragColor;

void main()
{
    if(u_Filled)
    {
        FragColor = vec4(u_Color.rgb , 0.2f);
    }
    else if(u_HighlightPass)
    {
        FragColor = u_Color;
    }
}