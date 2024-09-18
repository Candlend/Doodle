#type vertex
#version 450

layout(location = 0) in vec3 a_PositionOS;

out vec2 v_TexCoord;

void main()
{
	gl_Position = vec4(a_PositionOS, 1.0);
    v_TexCoord = (a_PositionOS.xy + 1.0) / 2.0;
}

#type fragment
#version 450

layout(location = 0) out vec4 FinalColor;

in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D u_Texture;

uniform float u_FilterRadius;

void main()
{
    float x = u_FilterRadius;
    float y = u_FilterRadius;

    // Take 9 samples around current texel:
    // a - b - c
    // d - e - f
    // g - h - i
    // === ('e' is the current texel) ===
    vec3 a = texture(u_Texture, v_TexCoord + vec2(-x, y)).rgb;
    vec3 b = texture(u_Texture, v_TexCoord + vec2(0, y)).rgb;
    vec3 c = texture(u_Texture, v_TexCoord + vec2(x, y)).rgb;
    vec3 d = texture(u_Texture, v_TexCoord + vec2(-x, 0)).rgb;
    vec3 e = texture(u_Texture, v_TexCoord).rgb;
    vec3 f = texture(u_Texture, v_TexCoord + vec2(x, 0)).rgb;
    vec3 g = texture(u_Texture, v_TexCoord + vec2(-x, -y)).rgb;
    vec3 h = texture(u_Texture, v_TexCoord + vec2(0, -y)).rgb;
    vec3 i = texture(u_Texture, v_TexCoord + vec2(x, -y)).rgb;

    vec3 color = e * 4.0;
    color += (b + d + f + h) * 2.0;
    color += (a + c + g + i);
    color *= 1.0 / 16.0;

    FinalColor = vec4(color, 1.0);
}