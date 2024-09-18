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

uniform float u_BloomStrength = 0.04f;
uniform float u_Exposure = 1.0f;
uniform sampler2D u_BloomTexture;

vec3 FilmicToneMapping(vec3 color) {
    color *= u_Exposure;
    color = (color * (color + vec3(0.004)) ) / (color * (color + vec3(1.0)));
    color = pow(color, vec3(1.0 / 2.2));
    return color;
}

void main()
{
    vec3 color = texture(u_Texture, v_TexCoord).rgb;
    vec3 bloom = texture(u_BloomTexture, v_TexCoord).rgb;
    color = mix(color, bloom, vec3(u_BloomStrength));
    color = FilmicToneMapping(color);
    FinalColor = vec4(color, 1.0);
}