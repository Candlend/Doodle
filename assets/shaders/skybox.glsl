#type vertex
#version 430

layout(location = 0) in vec3 a_Position;

out vec3 v_Position;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    v_Position = a_Position;
    vec4 pos = u_Projection * u_View * vec4(a_Position, 1.0); 
    gl_Position = pos.xyww;
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;

in vec3 v_Position;

uniform samplerCube u_Skybox;
uniform float u_Intensity;
uniform float u_Rotation;

vec3 RotateVectorAboutY(float angle, vec3 vec)
{
    angle = radians(angle);
    mat3x3 rotationMatrix ={vec3(cos(angle),0.0,sin(angle)),
                            vec3(0.0,1.0,0.0),
                            vec3(-sin(angle),0.0,cos(angle))};
    return rotationMatrix * vec;
}

void main()
{
    // Sample the cube map texture using the fragment's position
    vec3 rotatedPosition = RotateVectorAboutY(-u_Rotation, v_Position);
    vec3 color = textureLod(u_Skybox, normalize(rotatedPosition), 0).rgb * u_Intensity;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 
    finalColor = vec4(color, 1.0);
}
