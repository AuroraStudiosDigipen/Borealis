#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;  // Vertex position

uniform mat4 u_ViewProjection;
uniform mat4 u_ModelTransform;

void main()
{
    gl_Position = u_ViewProjection * u_ModelTransform * vec4(a_Position, 1.0);
}

#type fragment
#version 410 core	

uniform vec4 u_Color; //uniform color for now

out vec4 FragColor;

void main()
{
    FragColor = u_Color;
}