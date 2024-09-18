#type vertex
#version 450

layout(location = 0) in vec3 a_Position;

out vec2 v_TexCoord;

void main()
{
	gl_Position = vec4(a_Position, 1.0);
    v_TexCoord = (a_Position.xy + 1.0) / 2.0;
}

#type fragment
#version 450

layout(location = 0) out vec4 finalColor;

in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D u_Texture;

void main()
{
    vec2 texelSize = 1.0 / textureSize(u_Texture, 0);
    float x = texelSize.x;
    float y = texelSize.y;

    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // === ('e' is the current texel) ===
    vec3 a = texture(u_Texture, v_TexCoord + vec2(-2*x, 2*y)).rgb;
    vec3 b = texture(u_Texture, v_TexCoord + vec2(0, 2*y)).rgb;
    vec3 c = texture(u_Texture, v_TexCoord + vec2(2*x, 2*y)).rgb;
    vec3 d = texture(u_Texture, v_TexCoord + vec2(-2*x, 0)).rgb;
    vec3 e = texture(u_Texture, v_TexCoord).rgb;
    vec3 f = texture(u_Texture, v_TexCoord + vec2(2*x, 0)).rgb;
    vec3 g = texture(u_Texture, v_TexCoord + vec2(-2*x, -2*y)).rgb;
    vec3 h = texture(u_Texture, v_TexCoord + vec2(0, -2*y)).rgb;
    vec3 i = texture(u_Texture, v_TexCoord + vec2(2*x, -2*y)).rgb;
    vec3 j = texture(u_Texture, v_TexCoord + vec2(-x, y)).rgb;
    vec3 k = texture(u_Texture, v_TexCoord + vec2(x, y)).rgb;
    vec3 l = texture(u_Texture, v_TexCoord + vec2(-x, -y)).rgb;
    vec3 m = texture(u_Texture, v_TexCoord + vec2(x, -y)).rgb;

    vec3 color = e * 0.125;
    color += (a+c+g+i) * 0.03125;
    color += (b+d+f+h) * 0.0625;
    color += (j+k+l+m) * 0.125;

    finalColor = vec4(color, 1.0);
}