#type vertex
#version 430

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

out vec3 v_Color;
out vec2 v_TexCoord;

void main()
{
	gl_Position = vec4(a_Position, 1.0);
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;

in vec3 v_Color;
in vec2 v_TexCoord;

uniform vec4 u_Color;
uniform bool u_Debug;
uniform sampler2D u_Texture;

void main()
{
	if (u_Debug)
	{
		finalColor = vec4(1.0, 1.0, 1.0, 1.0);
		return;
	}
	finalColor = u_Color * vec4(v_Color, 1.0) * texture(u_Texture, v_TexCoord);
}