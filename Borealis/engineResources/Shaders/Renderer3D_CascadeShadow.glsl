#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;  // Vertex position
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

uniform mat4 u_ModelTransform;

//Animation variables
uniform bool u_HasAnimation;
uniform int  u_AnimationIndex;
const int MAX_BONES = 128;
const int MAX_BONE_INFLUENCE = 4;
const int MAX_ANIMATIONS = 5;

struct Animation
{
    mat4 FinalBonesMatrices[MAX_BONES];
};

layout(std140) uniform AnimationUBO
{
    Animation animations[MAX_ANIMATIONS];
};

void main()
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

		gl_Position = u_ModelTransform * TotalPosition;
	}
    else
    {
        gl_Position = u_ModelTransform * vec4(a_Position, 1.0);
    }
}

#type geometry
#version 410 core	

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 u_LightSpaceMatrices[4];

void main()
{          
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = 
            u_LightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}

#type fragment
#version 410 core	

void main()
{

}