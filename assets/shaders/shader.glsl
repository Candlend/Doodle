#type vertex
#version 430

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

out vec4 v_Color;

void main()
{
	gl_Position = vec4(a_Position, 1.0);
	v_Color = a_Color;
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;

in vec4 v_Color;
uniform vec4 u_Color;

void main()
{
	finalColor = u_Color * v_Color;
}