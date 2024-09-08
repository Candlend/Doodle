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
    mat3 normalMatrix = mat3(transpose(inverse(u_Model))); // TODO 放在CPU端计算
    v_Normal = normalMatrix * a_Normal; 
    // Transform position to world space
    v_Position = vec3(u_Model * vec4(a_Position, 1.0));
    
    // Compute TBN matrix
    vec3 T = normalMatrix * a_Tangent;
    vec3 B = normalMatrix * a_Binormal;
    v_TBN = mat3(T, B, v_Normal);
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_Position;
in mat3 v_TBN;

uniform vec4 u_AlbedoColor;
uniform float u_NormalScale;
uniform float u_Metallic;
uniform float u_Roughness;
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
    vec3 CameraPosition;
} u_Scene;

struct PointLight
{
    vec3 Position;
    vec3 Radiance;
    float Intensity;
    float MinRadius;
    float Radius;
    float Falloff; // TODO 未使用
    float SourceSize; // TODO 未使用
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
    float Falloff; // TODO 未使用
};

layout(std140, binding = 2) uniform SpotLightData
{
    uint LightCount;
    SpotLight Lights[256];
} u_SpotLights;

// Function to calculate the Fresnel-Schlick approximation
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Function to compute the Specular reflection using the Cook-Torrance model
vec3 CookTorranceBRDF(vec3 normal, vec3 viewDir, vec3 lightDir, float metallic, float roughness)
{
    // Compute half-vector
    vec3 halfDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfDir), 0.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotV = max(dot(normal, viewDir), 0.0);
    
    // Calculate the Fresnel reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04), u_AlbedoColor.rgb, metallic);
    vec3 F = FresnelSchlick(NdotV, F0);
    
    // Calculate D (the normal distribution)
    float alpha = roughness * roughness;
    float D = (alpha * alpha) / (3.141592 * pow(NdotH * NdotH * (alpha * alpha - 1.0) + 1.0, 2.0));
    
    // Calculate G (the geometric attenuation)
    float G = min(1.0, min((2.0 * NdotH * NdotV) / dot(viewDir, halfDir), (2.0 * NdotH * NdotL) / dot(lightDir, halfDir)));
    
    // Calculate the specular color
    return (D * G * F) / max(3.14 * NdotV * NdotL, 0.00390625);;
}

void main()
{
    // Sample textures
    vec4 albedo = texture(u_AlbedoTexture, v_TexCoord) * u_AlbedoColor;
    float metallic = texture(u_MetallicTexture, v_TexCoord).r * u_Metallic;
    float roughness = texture(u_RoughnessTexture, v_TexCoord).r * u_Roughness;

    // Transform normal from tangent space to world space
    vec3 normal = normalize(v_TBN * (texture(u_NormalTexture, v_TexCoord).xyz * 2.0 - 1.0) * u_NormalScale);
    
    // Calculate view direction
    vec3 viewDir = normalize(u_Scene.CameraPosition - v_Position);
    
    // Initialize color
    vec3 color = vec3(0.0);
    
    // Ambient lighting
    color += u_Scene.AmbientRadiance * albedo.rgb;

    // Directional light contribution
    vec3 lightDir = normalize(-u_Scene.DirectionalLight.Direction);
    color += CookTorranceBRDF(normal, viewDir, lightDir, metallic, roughness) * u_Scene.DirectionalLight.Radiance * u_Scene.DirectionalLight.Intensity;

    // Point lights
    for (uint i = 0; i < u_PointLights.LightCount; ++i)
    {
        PointLight light = u_PointLights.Lights[i];
        float distance = length(light.Position - v_Position);
        if (distance > light.Radius)
            continue;
        vec3 lightDir = normalize(light.Position - v_Position);
        // Attenuation
        float attenuation = clamp(1.0 - (distance - light.MinRadius) / (light.Radius - light.MinRadius), 0.0, 1.0); // TODO 参数可以调整
        color += CookTorranceBRDF(normal, viewDir, lightDir, metallic, roughness) * light.Radiance * light.Intensity * attenuation;
    }

    // Spot lights
    for (uint i = 0; i < u_SpotLights.LightCount; ++i)
    {
        SpotLight light = u_SpotLights.Lights[i];
        float distance = length(light.Position - v_Position);
        if (distance > light.Range)
            continue;
        vec3 lightDir = normalize(light.Position - v_Position);
        // Attenuation
        float attenuation = clamp(1.0 - (distance) / (light.Range), 0.0, 1.0);
        
        // Angle attenuation
        float angleAttenuation = smoothstep(cos(light.Angle), 1.0, dot(-lightDir, normalize(light.Direction)));  // TODO 参数可以调整
        attenuation *= angleAttenuation;
        
        color += CookTorranceBRDF(normal, viewDir, lightDir, metallic, roughness) * light.Radiance * light.Intensity * attenuation;
    }

    // Final color output
    finalColor = vec4(color, 1.0);
}