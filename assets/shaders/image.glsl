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
    finalColor = texture(u_Texture, v_TexCoord);
}