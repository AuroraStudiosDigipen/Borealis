#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in ivec4 boneIds; 
layout(location = 4) in vec4 weights;

uniform mat4 u_ModelTransform;
uniform mat4 u_ViewProjection;

const int MAX_BONES = 300;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 u_FinalBonesMatrices[MAX_BONES];

out vec2 v_TexCoord;

void main()
{
	vec4 TotalPosition = vec4(0.f);

	for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >= MAX_BONES) 
        {
            TotalPosition = vec4(a_Position,1.0f);
            break;
        }
        vec4 localPosition = u_FinalBonesMatrices[boneIds[i]] * vec4(a_Position,1.0f);
        TotalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(u_FinalBonesMatrices[boneIds[i]]) * a_Normal;
    }

	v_TexCoord = a_TexCoord;
	
	gl_Position = u_ViewProjection * u_ModelTransform * TotalPosition;	
	//gl_Position = u_ViewProjection * u_ModelTransform * vec4(a_Position, 1.0);	
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