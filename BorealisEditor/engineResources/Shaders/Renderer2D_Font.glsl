#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_EntityID;
layout(location = 4) in int a_Outline;
layout(location = 5) in float a_Width;

//uniform mat4 u_ViewProjection;
layout(std140) uniform Camera
{
	mat4 u_ViewProjection;
};


out vec2 v_TexCoord;
out vec4 v_Color;
flat out int v_EntityID;

flat out int v_outline;
out float v_width;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	

	v_outline = a_Outline;
	v_width = a_Width;
}

#type fragment
#version 410 core			
layout(location = 0) out vec4 color;
layout(location = 1) out int entityIDs;

in vec2 v_TexCoord;
in vec4 v_Color;
flat in int v_EntityID;
flat in int v_outline;
in float v_width;
			
uniform sampler2D u_Texture;

// uniform bool u_outline;
// uniform float u_Width;// = 0.05;
float u_bias = 0.85f;

layout(std140) uniform SceneRenderUBO
{
    float u_Threshold;
    float u_Knee;
    float u_SampleScale;
    float exposure;
};

float screenPxRange() {
	float pxRange = 1000.f; // set to distance field's pixel range
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

    float outlineWidth = 2.0 * mix(0.75, v_width, 0.5);

	if(v_outline == 0)
	{
		outlineWidth = 0.f;
		u_bias = 0.5f;
	}

    float inner = width * (sd - u_bias) + u_bias;
    float outer = width * (sd - u_bias + outlineWidth) + u_bias;

    float inner_opacity = clamp(inner, 0.0, 1.0);
    vec4 inner_color = v_Color;
    float outer_opacity = clamp(outer, 0.0, 1.0);
    vec4 outer_color = vec4(0, 0, 0, 1); 

    color = (inner_color * inner_opacity) + (outer_color * outer_opacity);
}