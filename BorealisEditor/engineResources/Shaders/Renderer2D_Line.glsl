#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

//uniform mat4 u_ViewProjection;

layout(std140) uniform Camera
{
	mat4 u_ViewProjection;
};


out vec4 v_Color;

void main()
{
	v_Color = a_Color;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
}

#type fragment
#version 410 core			
layout(location = 0) out vec4 o_color;

in vec4 v_Color;


void main()
{
	o_color = v_Color;
}