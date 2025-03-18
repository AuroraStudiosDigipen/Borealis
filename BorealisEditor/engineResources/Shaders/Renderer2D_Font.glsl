#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_EntityID;

//uniform mat4 u_ViewProjection;
layout(std140) uniform Camera
{
	mat4 u_ViewProjection;
};


out vec2 v_TexCoord;
out vec4 v_Color;
flat out int v_EntityID;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
}

#type fragment
#version 410 core			
layout(location = 0) out vec4 color;
layout(location = 1) out int entityIDs;

in vec2 v_TexCoord;
in vec4 v_Color;
flat in int v_EntityID;
			
uniform sampler2D u_Texture;

float u_outline = 0.23;

float screenPxRange() {
	float pxRange = 100.f; // set to distance field's pixel range
    vec2 unitRange = vec2(pxRange)/vec2(textureSize(u_Texture, 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(v_TexCoord);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
    entityIDs = v_EntityID;

    vec3 msd = texture(u_Texture, v_TexCoord).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float width = screenPxRange();

    float relativeOutline = 48.0 / 16.0; 
    float outlineWidth = mix(0.75, u_outline, 0.5);

    float inner = width * (sd - 0.5) + 0.5;
    float outer = width * (sd - 0.5 + outlineWidth) + 0.5;

    float inner_opacity = clamp(inner, 0.0, 1.0);
    vec4 inner_color = vec4(1, 1, 1, 1);
    float outer_opacity = clamp(outer, 0.0, 1.0);
    vec4 outer_color = vec4(0, 0, 0, 1); 

    color = (inner_color * inner_opacity) + (outer_color * outer_opacity);

	// entityIDs = v_EntityID;

	// vec3 msd = texture(u_Texture, v_TexCoord).rgb;
	// float sd = median(msd.r, msd.g, msd.b);
	// float screenPxDistance = screenPxRange()*(sd - 0.5);
	// float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
	// if (opacity == 0.0)
	// 	discard;

	// vec4 bgColor = vec4(0.0);
    // color = mix(bgColor, v_Color, opacity);	
	// if (color.a == 0.0)
	// 	discard;
}