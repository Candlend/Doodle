#type vertex
#version 430

layout(location = 0) in vec3 a_Position;

void main()
{
	gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;

void main()
{
	finalColor = vec4(0.2, 0.4, 1.0, 1.0);
}