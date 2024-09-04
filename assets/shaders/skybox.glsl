#type vertex
#version 430

layout(location = 0) in vec3 a_Position;

out vec3 v_Position;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    // Pass the position to the fragment shader
    v_Position = a_Position;

    // Set the position of the vertex in clip space
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0); // TODO 之后会去掉u_Model
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;

in vec3 v_Position;

uniform samplerCube u_Skybox;

void main()
{
    // Sample the cube map texture using the fragment's position
    finalColor = texture(u_Skybox, normalize(v_Position));
}
