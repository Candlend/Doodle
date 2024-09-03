#type vertex
#version 430

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_Position;
out mat3 v_TBN; // 切线-法线-切线空间矩阵

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
    
    // Transform normal to world space
    v_Normal = normalize(mat3(u_Model) * a_Normal);
    
    // Transform position to world space
    v_Position = vec3(u_Model * vec4(a_Position, 1.0));
    
    // Compute TBN matrix
    vec3 T = normalize(mat3(u_Model) * a_Tangent);
    vec3 B = normalize(mat3(u_Model) * a_Binormal);
    v_TBN = mat3(T, B, v_Normal);
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_Position;
in mat3 v_TBN;

uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_MetallicTexture;
uniform sampler2D u_RoughnessTexture;

struct DirectionalLight
{
    vec3 Direction;
    vec3 Radiance;
    float Intensity;
};

layout(std140, binding = 0) uniform SceneData
{
    DirectionalLight DirectionalLight;
    vec3 AmbientRadiance;
} u_Scene;

struct PointLight
{
    vec3 Position;
    vec3 Radiance;
    float Intensity;
    float MinRadius;
    float Radius;
    float Falloff;
    float SourceSize;
};

layout(std140, binding = 1) uniform PointLightData
{
    uint LightCount;
    PointLight Lights[256];
} u_PointLights;

struct SpotLight
{
    vec3 Position;
    vec3 Direction;
    vec3 Radiance;
    float Intensity;
    float AngleAttenuation;
    float Range;
    float Angle;
    float Falloff;
};

layout(std140, binding = 2) uniform SpotLightData
{
    uint LightCount;
    SpotLight Lights[256];
} u_SpotLights;

void main()
{
    vec3 albedo = texture(u_AlbedoTexture, v_TexCoord).rgb;
    vec3 normal = texture(u_NormalTexture, v_TexCoord).rgb;
    float metallic = texture(u_MetallicTexture, v_TexCoord).r;
    float roughness = texture(u_RoughnessTexture, v_TexCoord).r;
    finalColor = vec4(albedo, 1.0);
}