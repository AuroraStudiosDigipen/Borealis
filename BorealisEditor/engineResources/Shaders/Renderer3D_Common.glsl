#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;  // Vertex position
layout(location = 1) in vec3 a_Normal;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

//uniform mat4 u_ViewProjection;
layout(std140) uniform Camera
{
	mat4 u_ViewProjection;
};
uniform mat4 u_ModelTransform;

uniform bool u_HasAnimation;
uniform int  u_AnimationIndex;
const int MAX_BONES = 128;
const int MAX_BONE_INFLUENCE = 4;
const int MAX_ANIMATIONS = 6;

struct Animation
{
    mat4 FinalBonesMatrices[MAX_BONES];
};

layout(std140) uniform AnimationUBO
{
    Animation animations[MAX_ANIMATIONS];
};

void MainPass()
{
    if(u_HasAnimation)
    {	
		//animation
		vec4 TotalPosition = vec4(0.f);

		//vec3 weightedNormal = vec3(0.0);
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(boneIds[i] == -1) 
                continue;
            if(boneIds[i] >= MAX_BONES) 
            {
                TotalPosition = vec4(a_Position,1.0f);
                break;
            }
            vec4 localPosition = animations[u_AnimationIndex].FinalBonesMatrices[boneIds[i]] * vec4(a_Position,1.0f);
            TotalPosition += localPosition * weights[i];
        }

		gl_Position = u_ViewProjection * u_ModelTransform * TotalPosition;	
	}
	else
	{
        gl_Position = u_ViewProjection * u_ModelTransform * vec4(a_Position, 1.0);
	}
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
    else
    {
        FragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}