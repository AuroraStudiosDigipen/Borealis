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

uniform sampler2D opaqueTex;
uniform sampler2D accumColorTex;
uniform sampler2D accumAlphaTex;

void main()
{
    vec4 accumColor = texture(accumColorTex, v_TexCoord);
    float accumAlpha = texture(accumAlphaTex, v_TexCoord).r;

    vec3 transparentColor = accumColor.rgb / clamp(accumColor.a, 1e-4, 5e4);
    vec4 opaque = texture(opaqueTex, v_TexCoord);

    vec3 finalColor = mix(opaque.rgb, transparentColor, accumAlpha);
    FragColor = vec4(finalColor, 1.0);
}