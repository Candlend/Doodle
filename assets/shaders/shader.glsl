#type vertex
#version 430

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
	v_TexCoord = a_TexCoord;
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;

in vec2 v_TexCoord;

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
	finalColor = texture(u_Texture, v_TexCoord);
}