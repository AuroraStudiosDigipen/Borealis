#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;  // Vertex position

uniform mat4 u_ModelTransform;

void main()
{
    gl_Position = u_ModelTransform * vec4(a_Position, 1.0);
}

#type geometry
#version 410 core	

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 u_LightSpaceMatrices;

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