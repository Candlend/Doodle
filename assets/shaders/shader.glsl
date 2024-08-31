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
} u_Scene;

struct PointLight
{
    vec3 Position;
    vec3 Radiance;
    float Intensity;
};

layout(std140, binding = 1) uniform PointLightData
{
    uint LightCount;
    PointLight Lights[512];
} u_PointLights;

struct SpotLight
{
    vec3 Position;
    vec3 Direction;
    vec3 Radiance;
    float Intensity;
};

layout(std140, binding = 2) uniform SpotLightData
{
    uint LightCount;
    SpotLight Lights[512];
} u_SpotLights;

// 环境光常量
const vec3 AMBIENT_LIGHT = vec3(0.1); // 你可以根据需要调整这个值

// Function to calculate Fresnel-Schlick approximation
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Function to calculate the geometric factor
float geometricSmith(float NdotV, float NdotL, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotL / (NdotL * (1.0 - k) + k) * NdotV / (NdotV * (1.0 - k) + k);
}

// Function to calculate the BRDF
vec3 pbr(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, float metallic, float roughness)
{
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = fresnelSchlick(max(dot(viewDir, lightDir), 0.0), F0);
    
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotV = max(dot(normal, viewDir), 0.0);
    
    // Geometric attenuation
    float G = geometricSmith(NdotV, NdotL, roughness);
    
    // Cook-Torrance BRDF
    return (F * G * NdotL) / (4.0 * NdotL * NdotV + 0.001);
}

void main()
{
    vec3 albedo = texture(u_AlbedoTexture, v_TexCoord).rgb;
    
    // 从法线贴图中获取切线空间法线
    vec3 tangentNormal = texture(u_NormalTexture, v_TexCoord).rgb * 2.0 - 1.0;
    tangentNormal = normalize(tangentNormal);

    // 将切线空间法线转换到世界空间
    vec3 normal = normalize(v_TBN * tangentNormal);

    // 计算光照
    vec3 finalLighting = vec3(0.0);
    vec3 viewDir = normalize(-v_Position); // Assuming camera is at the origin

    // 环境光计算
    vec3 ambient = AMBIENT_LIGHT * albedo;

    // Directional Light
    vec3 lightDir = normalize(-u_Scene.DirectionalLight.Direction);
    float metallic = texture(u_MetallicTexture, v_TexCoord).r;
    float roughness = texture(u_RoughnessTexture, v_TexCoord).r;
    finalLighting += pbr(normal, lightDir, viewDir, albedo, metallic, roughness) * u_Scene.DirectionalLight.Radiance * u_Scene.DirectionalLight.Intensity;

    // Point Lights
    for (uint i = 0; i < u_PointLights.LightCount; ++i)
    {
        PointLight light = u_PointLights.Lights[i];
        vec3 lightDir = normalize(light.Position - v_Position);
        finalLighting += pbr(normal, lightDir, viewDir, albedo, metallic, roughness) * light.Radiance * light.Intensity;
    }

    // Spot Lights
    for (uint i = 0; i < u_SpotLights.LightCount; ++i)
    {
        SpotLight light = u_SpotLights.Lights[i];
        vec3 lightDir = normalize(light.Position - v_Position);
        finalLighting += pbr(normal, lightDir, viewDir, albedo, metallic, roughness) * light.Radiance * light.Intensity;
    }

    finalColor = vec4(finalLighting + ambient, 1.0); // 将环境光添加到最终颜色
}