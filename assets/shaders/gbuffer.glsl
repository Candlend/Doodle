#type vertex
#version 450

layout(location = 0) in vec3 a_PositionOS;
layout(location = 1) in vec3 a_NormalOS;
layout(location = 2) in vec3 a_TangentOS;
layout(location = 3) in vec3 a_BinormalOS;
layout(location = 4) in vec2 a_TexCoord;

out Varyings
{
    vec2 TexCoord;
    vec3 NormalWS;
    vec3 PositionWS;
    mat3 TBN; 
    vec4 PositionHLS;
} vs_out;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_LightSpaceMatrix;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(a_PositionOS, 1.0);
    vs_out.TexCoord = a_TexCoord;
    
    // Transform normal to world space
    mat3 normalModel = mat3(transpose(inverse(u_Model))); // TODO 放在CPU端计算
    vs_out.NormalWS = normalModel * a_NormalOS; 
    // Transform position to world space
    vs_out.PositionWS = vec3(u_Model * vec4(a_PositionOS, 1.0));
    
    // Compute TBN matrix
    vec3 T = normalModel * a_TangentOS;
    vec3 B = normalModel * a_BinormalOS;
    vs_out.TBN = mat3(T, B, vs_out.NormalWS);

    // Calculate light space position
    vs_out.PositionHLS = u_LightSpaceMatrix * vec4(vs_out.PositionWS, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;

uniform float u_NearPlane;
uniform float u_FarPlane;

float LinearizeDepth(float depth) // TODO 没有考虑正交相机
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * u_NearPlane * u_FarPlane) / (u_FarPlane + u_NearPlane - z * (u_FarPlane - u_NearPlane));
}

in Varyings
{
    vec2 TexCoord;
    vec3 NormalWS;
    vec3 PositionWS;
    mat3 TBN;
    vec4 PositionHLS;
} fs_in;

uniform float u_NormalScale;
uniform sampler2D u_NormalTexture;

void main()
{
    gPosition = vec4(fs_in.PositionWS, LinearizeDepth(gl_FragCoord.z));
    gNormal.xyz = normalize(fs_in.TBN * (texture(u_NormalTexture, fs_in.TexCoord).xyz * 2.0 - 1.0) * u_NormalScale);
    gNormal.w = 1.0;
}