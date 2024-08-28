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

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
    v_Normal = normalize(mat3(u_Model) * a_Normal); // Transform normal to world space
    v_Position = vec3(u_Model * vec4(a_Position, 1.0)); // Transform position to world space
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_Position;

uniform sampler2D u_Texture;

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
    float MinRadius;
    float Radius;
    float Falloff;
    float SourceSize;
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
    float AngleAttenuation;
    float Range;
    float Angle;
    float Falloff;
};

layout(std140, binding = 2) uniform SpotLightData
{
    uint LightCount;
    SpotLight Lights[512];
} u_SpotLights;

void main()
{
    vec3 color = texture(u_Texture, v_TexCoord).rgb;
    vec3 finalLighting = vec3(0.0);

    // Directional Light
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(-u_Scene.DirectionalLight.Direction);
    float diff = max(dot(norm, lightDir), 0.0);
    finalLighting += diff * u_Scene.DirectionalLight.Radiance * u_Scene.DirectionalLight.Intensity;

    // Point Lights
    for (uint i = 0; i < u_PointLights.LightCount; ++i)
    {
        PointLight light = u_PointLights.Lights[i];
        vec3 lightDir = light.Position - v_Position;
        float distance = length(lightDir);
        lightDir = normalize(lightDir);
        
        // Attenuation
        float attenuation = clamp(1.0 - (distance / light.Radius), 0.0, 1.0);
        float diff = max(dot(norm, lightDir), 0.0);
        finalLighting += diff * light.Radiance * light.Intensity * attenuation;
    }

    // // Spot Lights
    // for (uint i = 0; i < u_SpotLights.LightCount; ++i)
    // {
    //     SpotLight light = u_SpotLights.Lights[i];
    //     vec3 lightDir = light.Position - v_Position;
    //     float distance = length(lightDir);
    //     lightDir = normalize(lightDir);
        
    //     // Attenuation
    //     float attenuation = clamp(1.0 - (distance / light.Range), 0.0, 1.0);
        
    //     // Angle attenuation
    //     float angle = dot(normalize(light.Direction), -lightDir);
    //     float spotAttenuation = smoothstep(cos(light.Angle), 1.0, angle);
    //     attenuation *= spotAttenuation;

    //     float diff = max(dot(norm, lightDir), 0.0);
    //     finalLighting += diff * light.Radiance * light.Intensity * attenuation;
    // }

    finalColor = vec4(u_Scene.DirectionalLight.Radiance * u_Scene.DirectionalLight.Intensity * color, 1.0);
}
