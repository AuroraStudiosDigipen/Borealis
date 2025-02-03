#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;
layout(location = 6) in int a_BillBoarding;

//uniform mat4 u_ViewProjection;
layout(std140) uniform Camera
{
	mat4 u_ViewProjection;
	vec4 u_CameraPos;
};


out vec2 		g_TexCoord;
out vec4 		g_Color;
flat out int 	g_TexIndex;
out float 		g_TilingFactor;
flat out int 	g_EntityID;
out int			g_BillBoarding;

void main()
{
	g_Color = a_Color;
	g_TexCoord = a_TexCoord;
	g_TexIndex = a_TexIndex;
	g_TilingFactor = a_TilingFactor;
	g_EntityID = a_EntityID;
	g_BillBoarding = a_BillBoarding;

	if(a_BillBoarding == 1)
	{
		gl_Position = vec4(a_Position, 1.0);	
	}
	else
	{
		gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
	}
	
}

#type geometry
#version 410 core	
layout(triangles, invocations = 1) in; 
layout(triangle_strip, max_vertices = 4) out; // Emit a quad (2 triangles = 4 vertices).

layout(std140) uniform Camera
{
	mat4 u_ViewProjection; // View-projection matrix.
	vec4 u_CameraPos;      // Camera position in world space.
};

in vec2 		g_TexCoord[];
in vec4 		g_Color[];
flat in int 	g_TexIndex[];
in float 		g_TilingFactor[];
flat in int 	g_EntityID[];
in int			g_BillBoarding[];

out vec2 		v_TexCoord;
out vec4 		v_Color;
flat out int 	v_TexIndex;
out float 		v_TilingFactor;
flat out int 	v_EntityID;

void main()
{      
	if (g_BillBoarding[0] == 0) // Check if billboard rendering is disabled
    {
        // Pass-through: emit the triangle vertices as-is
        for (int i = 0; i < 3; ++i)
        {
            gl_Position = gl_in[i].gl_Position; // Pass the original position
            v_TexCoord = g_TexCoord[i];        // Pass texture coordinates
            v_Color = g_Color[i];              // Pass color
            v_TexIndex = g_TexIndex[i];        // Pass texture index
            v_TilingFactor = g_TilingFactor[i]; // Pass tiling factor
            v_EntityID = g_EntityID[i];        // Pass entity ID
            EmitVertex();
        }
        EndPrimitive();
    }
	else
	{
		// Calculate billboard center (using the first vertex position)
		vec3 pos = gl_in[0].gl_Position.xyz;

		// Calculate camera-to-point direction
		vec3 cameraToPoint = normalize(pos - u_CameraPos.xyz);

		vec3 worldUp = vec3(0.0, 1.0, 0.0);
		if (abs(dot(cameraToPoint, worldUp)) > 0.999)
		{
			worldUp = vec3(0.0, 0.0, 1.0); // Use alternative up (e.g., forward)
		}

		// Calculate orthogonal right and up vectors
		vec3 right = normalize(cross(worldUp, cameraToPoint));
		vec3 up = normalize(cross(cameraToPoint, right));

		// Scale the billboard quad
		float size = 1.0; // Adjust billboard size as needed
		right *= size * 0.5; // Half-size to define quad's extent
		up *= size * 0.5;

		// Emit the four corners of the quad
		vec3 corners[4];
		corners[0] = pos - right - up; // Bottom-left
		corners[1] = pos + right - up; // Bottom-right
		corners[2] = pos - right + up; // Top-left
		corners[3] = pos + right + up; // Top-right

		vec2 texCoords[4] = vec2[](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0));

		// Emit vertices
		for (int i = 0; i < 4; ++i)
		{
			gl_Position = u_ViewProjection * vec4(corners[i], 1.0); // Transform to clip space
			v_TexCoord = texCoords[i];                             // Pass texture coordinates
			v_Color = g_Color[0];                                  // Pass color
			v_TexIndex = g_TexIndex[0];                            // Pass texture index
			v_TilingFactor = g_TilingFactor[0];                    // Pass tiling factor
			v_EntityID = g_EntityID[0];                            // Pass entity ID
			EmitVertex();
		}

		EndPrimitive();
	}
    
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