#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

out Varyings
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 Position;
    mat3 TBN; 
    vec4 LightSpacePos;
} vs_out;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_LightSpaceMatrix;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
    vs_out.TexCoord = a_TexCoord;
    
    // Transform normal to world space
    mat3 normalMatrix = mat3(transpose(inverse(u_Model))); // TODO 放在CPU端计算
    vs_out.Normal = normalMatrix * a_Normal; 
    // Transform position to world space
    vs_out.Position = vec3(u_Model * vec4(a_Position, 1.0));
    
    // Compute TBN matrix
    vec3 T = normalMatrix * a_Tangent;
    vec3 B = normalMatrix * a_Binormal;
    vs_out.TBN = mat3(T, B, vs_out.Normal);

    // Calculate light space position
    vs_out.LightSpacePos = u_LightSpaceMatrix * vec4(vs_out.Position, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;

in Varyings
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 Position;
    mat3 TBN;
    vec4 LightSpacePos;
} fs_in;

uniform float u_NormalScale;
uniform sampler2D u_NormalTexture;

void main()
{
    gPosition = vec4(fs_in.Position, 1.0);
    gNormal.xyz = normalize(fs_in.TBN * (texture(u_NormalTexture, fs_in.TexCoord).xyz * 2.0 - 1.0) * u_NormalScale);
    gNormal.w = 1.0;
}