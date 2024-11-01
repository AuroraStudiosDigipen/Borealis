#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

uniform mat4 u_ModelTransform;
uniform mat4 u_ViewProjection;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * u_ModelTransform * vec4(a_Position, 1.0);	
}

#type fragment
#version 410 core			
layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
			
uniform sampler2D u_Texture;

void main()
{
	color = texture(u_Texture, v_TexCoord);
}