#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;

uniform mat4 u_ViewProjection;
// layout(std140) uniform Camera
// {
// 	mat4 u_ViewProjection;
// };


out vec2 v_TexCoord;
out vec4 v_Color;
flat out int v_TexIndex;
out float v_TilingFactor;
flat out int v_EntityID;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_TilingFactor = a_TilingFactor;
	v_EntityID = a_EntityID;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
}

#type fragment
#version 410 core			
layout(location = 0) out vec4 color;
layout(location = 1) out int entityIDs;

in vec2 v_TexCoord;
in vec4 v_Color;
flat in int v_TexIndex;
in float v_TilingFactor;
flat in int v_EntityID;
			
uniform sampler2D u_Texture[16];

void main()
{
	color = texture(u_Texture[v_TexIndex], v_TexCoord * v_TilingFactor) * v_Color;
	entityIDs = v_EntityID;
}