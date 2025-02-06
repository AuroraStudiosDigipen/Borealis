#type vertex
#version 410 core
			
layout(location = 0) in vec2 a_Position;  // Vertex position
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = vec4(a_Position, 0.f, 1.0);
}

#type fragment
#version 410 core	

in vec2 v_TexCoord;
out vec4 FragColor;

// uniform sampler2D opaqueTex;
// uniform sampler2D accumColorTex;
// uniform sampler2D accumAlphaTex;

// uniform bool u_CompositePass;

// void main()
// {
//     vec4 accumColor = texture(accumColorTex, v_TexCoord);

//     if(u_CompositePass)
//     {
//         float accumAlpha = texture(accumAlphaTex, v_TexCoord).r;
//         vec3 transparentColor = accumColor.rgb / clamp(accumColor.a, 1e-4, 5e4);
//         //transparentColor = pow(transparentColor, vec3(1.0/2.2)); 
//         FragColor = vec4(transparentColor, accumAlpha);
//     }
//     else
//     {
//         vec4 opaque = texture(opaqueTex, v_TexCoord);
//         vec3 transparentColor = accumColor.rgb;
//         vec3 finalColor = mix(opaque.rgb, transparentColor, accumColor.a);
//         FragColor = vec4(finalColor, 1.0);
//     }
// }

//uniform sampler2D opaqueTex;
uniform sampler2D accumColorTex;
uniform sampler2D accumAlphaTex;

// epsilon number
const float EPSILON = 0.00001f;

// calculate floating point numbers equality accurately
bool isApproximatelyEqual(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

// get the max value between three values
float max3(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

void main()
{
    float accumAlpha = texture(accumAlphaTex, v_TexCoord).r;

    // if (isApproximatelyEqual(accumAlpha, 1.0f))
    //     discard;

    vec4 accumColor = texture(accumColorTex, v_TexCoord);

    if (isinf(max3(abs(accumColor.rgb))))
        accumColor.rgb = vec3(accumColor.a);

    vec3 average_color = accumColor.rgb / max(accumColor.a, EPSILON);

    FragColor = vec4(average_color, 1.0f - accumAlpha);
}