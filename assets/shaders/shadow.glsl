#type vertex
#version 450

layout(location = 0) in vec3 a_PositionOS;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(a_PositionOS, 1.0);
}

#type fragment
#version 450

void main()
{
}