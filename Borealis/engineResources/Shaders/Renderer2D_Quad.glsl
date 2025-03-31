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


out vec2 		v_TexCoord;
out vec4 		v_Color;
flat out int 	v_TexIndex;
out float 		v_TilingFactor;
flat out int 	v_EntityID;
flat out int	v_BillBoarding;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_TilingFactor = a_TilingFactor;
	v_EntityID = a_EntityID;
	v_BillBoarding = a_BillBoarding;	

	vec3 position = a_Position;
    
    if (a_BillBoarding != 0)
    {
        // Calculate camera direction
        vec3 cameraPos = u_CameraPos.xyz;
        vec3 toCamera = normalize(cameraPos - position);
        
        // Calculate billboard axes
        vec3 worldUp = vec3(0.0, 1.0, 0.0);
        vec3 right = normalize(cross(worldUp, toCamera));
        vec3 up = normalize(cross(toCamera, right));
        
        vec2 quadOffset = (a_TexCoord - 0.5) * 2.0;
        quadOffset *= a_TilingFactor;
        position += right * quadOffset.x + up * quadOffset.y;
    }
    
    gl_Position = u_ViewProjection * vec4(position, 1.0);
}

#type fragment
#version 410 core			
layout(location = 0) out vec4 color;
layout(location = 1) out int entityIDs;
layout(location = 2) out float outRevealage;

in vec2 	v_TexCoord;
in vec4 	v_Color;
flat in int v_TexIndex;
in float 	v_TilingFactor;
flat in int v_EntityID;
flat in int v_BillBoarding;
			
uniform bool u_Transparent;
uniform sampler2D u_Texture[16];

void main()
{
    float tilingFactor = (v_BillBoarding == 1) ?  1.f : v_TilingFactor;
    vec4 finalColor = texture(u_Texture[v_TexIndex], v_TexCoord * tilingFactor) * v_Color;
	
	entityIDs = v_EntityID;

    if(u_Transparent)
    {
        float weight = clamp(pow(min(1.0, finalColor.a * 10.0) + 0.01, 3.0) * 1e4 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
        color = vec4(finalColor.rgb * finalColor.a * weight, finalColor.a * weight);
        outRevealage = finalColor.a;
    }
    else
    {
        color = finalColor;
    }
}