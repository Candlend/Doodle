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

uniform float u_BloomStrength = 0.04f;
uniform sampler2D u_BloomTexture;
uniform float u_Exposure = 1.0f;

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
    finalColor = vec4(color, 1.0);
}