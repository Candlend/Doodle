#type vertex
#version 450

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
#version 450

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

uniform samplerCube u_IrradianceMap;
uniform samplerCube u_PrefilterMap;
uniform sampler2D u_BrdfLUT;

const float PI = 3.14159265359;

uniform sampler2D u_LTC1; // for inverse M
uniform sampler2D u_LTC2; // GGX norm, fresnel, 0(unused), sphere

const float LUT_SIZE  = 64.0; // ltc_texture size
const float LUT_SCALE = (LUT_SIZE - 1.0)/LUT_SIZE;
const float LUT_BIAS  = 0.5/LUT_SIZE;

struct DirectionalLight
{
    vec3 Direction;
    vec3 Radiance;
    float Intensity;
};

layout(std140, binding = 0) uniform SceneData
{
    DirectionalLight DirectionalLights[4];
    vec3 CameraPosition;
    float EnvironmentIntensity;
    float EnvironmentRotation;
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

struct AreaLight
{
    vec3 Points[4];
	vec3 Radiance;
    float Intensity;
	bool TwoSided;
};

layout(std140, binding = 3) uniform AreaLightData
{
    uint AreaLightCount;
    AreaLight Lights[32];
} u_AreaLights;

vec3 IntegrateEdgeVec(vec3 v1, vec3 v2)
{
    // Using built-in acos() function will result flaws
    // Using fitting result for calculating acos()
    float x = dot(v1, v2);
    float y = abs(x);

    float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
    float b = 3.4175940 + (4.1616724 + y)*y;
    float v = a / b;

    float theta_sintheta = (x > 0.0) ? v : 0.5*inversesqrt(max(1.0 - x*x, 1e-7)) - v;

    return cross(v1, v2)*theta_sintheta;
}

vec3 LTC_Evaluate(vec3 N, vec3 V, vec3 P, mat3 Minv, vec3 points[4], bool twoSided) {
    // 构建TBN矩阵的三个基向量
    vec3 T1, T2;
    T1 = normalize(V - N * dot(V, N));
    T2 = cross(N, T1);

    // 依据TBN矩阵旋转光源
    Minv = Minv * transpose(mat3(T1, T2, N));

    // 多边形四个顶点
    vec3 L[4];

    // 通过逆变换矩阵将顶点变换于 受约余弦分布 中
    L[0] = Minv * (points[0] - P);
    L[1] = Minv * (points[1] - P);
    L[2] = Minv * (points[2] - P);
    L[3] = Minv * (points[3] - P);

    // use tabulated horizon-clipped sphere
    // 判断着色点是否位于光源之后
    vec3 dir = points[0] - P; // LTC 空间
    vec3 lightNormal = cross(points[1] - points[0], points[3] - points[0]);
    bool behind = (dot(dir, lightNormal) < 0.0);
    
    if (!behind && !twoSided)
        return vec3(0.0);
    
    // 投影至单位球面上
    L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);
    L[3] = normalize(L[3]);

    // 边缘积分
    vec3 vsum = vec3(0.0);
    vsum += IntegrateEdgeVec(L[0], L[1]);
    vsum += IntegrateEdgeVec(L[1], L[2]);
    vsum += IntegrateEdgeVec(L[2], L[3]);
    vsum += IntegrateEdgeVec(L[3], L[0]);

    // 计算正半球修正所需要的的参数
    float len = length(vsum);

    float z = vsum.z/len;
    if (behind)
        z = -z;

    vec2 uv = vec2(z*0.5f + 0.5f, len); // range [0, 1]
    uv = uv*LUT_SCALE + LUT_BIAS;

    // 通过参数获得几何衰减系数
    float scale = texture(u_LTC2, uv).w;

    float sum = len*scale;


    // 输出
    vec3 Lo_i = vec3(sum, sum, sum);
    return Lo_i;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Function to compute the Specular reflection using the Cook-Torrance model
vec3 CookTorranceBRDF(vec3 normal, vec3 viewDir, vec3 lightDir, float metallic, float roughness, vec4 albedo)
{
    // Compute half-vector
    vec3 halfDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfDir), 0.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotV = max(dot(normal, viewDir), 0.0);
    
    // Calculate the Fresnel reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04), albedo.rgb, metallic);
    vec3 F = FresnelSchlick(NdotH, F0);

    // Calculate the microfacet distribution
    float D = DistributionGGX(normal, halfDir, roughness);

    // Calculate the geometric attenuation
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    
    // Calculate the specular color
    vec3 nominator = D * G * F;
    float denominator = max(4.0 * NdotV * NdotL, 0.00390625); 
    vec3 specular = nominator / denominator; 

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    kD *= 1.0 - metallic;   

    // Calculate the diffuse color
    vec3 diffuse = kD * albedo.rgb / PI;

    // Calculate the final color
    vec3 color = (diffuse + specular) * NdotL;

    return color;
}

vec3 RotateVectorAboutY(float angle, vec3 vec)
{
    angle = radians(angle);
    mat3x3 rotationMatrix ={vec3(cos(angle),0.0,sin(angle)),
                            vec3(0.0,1.0,0.0),
                            vec3(-sin(angle),0.0,cos(angle))};
    return rotationMatrix * vec;
}

vec3 IBL(vec3 normal, vec3 viewDir, vec4 albedo, float metallic, float roughness)
{
    // IBL
    viewDir = RotateVectorAboutY(u_Scene.EnvironmentRotation, viewDir);
    vec3 N = normalize(normal);
    vec3 V = normalize(viewDir);
    vec3 R = reflect(-V, N);
    vec3 F0 = mix(vec3(0.04), albedo.rgb, metallic);

    vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness); 
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 irradiance = texture(u_IrradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo.rgb;

    vec3 prefilteredColor = textureLod(u_PrefilterMap, R, roughness * textureQueryLevels(u_PrefilterMap)).rgb;
    vec2 envBRDF = texture(u_BrdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    return kD * diffuse + specular;
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
    color += IBL(normal, viewDir, albedo, metallic, roughness) * u_Scene.EnvironmentIntensity;

    // Directional lights
    for (uint i = 0; i < 4; ++i)
    {
        DirectionalLight light = u_Scene.DirectionalLights[i];
        vec3 lightDir = normalize(-light.Direction);
        color += CookTorranceBRDF(normal, viewDir, lightDir, metallic, roughness, albedo) * light.Radiance * light.Intensity;
    }

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
        color += CookTorranceBRDF(normal, viewDir, lightDir, metallic, roughness, albedo) * light.Radiance * light.Intensity * attenuation;
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
        
        color += CookTorranceBRDF(normal, viewDir, lightDir, metallic, roughness, albedo) * light.Radiance * light.Intensity * attenuation;
    }

    // Area lights
    vec3 N = normal;
    vec3 V = viewDir;
    vec3 P = v_Position;

    // use roughness and sqrt(1-cos_theta) to sample M_texture
    float NdotV = max(dot(N, V), 0.0);

    vec2 uv = vec2(roughness, sqrt(1.0f - NdotV));
    uv = uv * LUT_SCALE + LUT_BIAS;

    // get 4 parameters for inverse_M
    vec4 t1 = texture(u_LTC1, uv);

    // Get 2 parameters for Fresnel calculation
    vec4 t2 = texture(u_LTC2, uv);

    mat3 Minv = mat3(
        vec3(t1.x, 0, t1.y),
        vec3(  0,  1,    0),
        vec3(t1.z, 0, t1.w)
    );

    vec3 F0 = mix(vec3(0.04), albedo.rgb, metallic);
    vec3 F = FresnelSchlickRoughness(NdotV, F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    for (uint i = 0; i < u_AreaLights.AreaLightCount; ++i)
    {
        AreaLight light = u_AreaLights.Lights[i];
		// Evaluate LTC shading
		vec3 diffuse = LTC_Evaluate(N, V, P, mat3(1), light.Points, light.TwoSided);
		vec3 specular = LTC_Evaluate(N, V, P, Minv, light.Points, light.TwoSided);
        diffuse *= albedo.rgb;
		// GGX BRDF shadowing and Fresnel
		// t2.x: shadowedF90 (F90 normally it should be 1.0)
		// t2.y: Smith function for Geometric Attenuation Term, it is dot(V or L, H).
		specular *= kS * t2.x + (1.0f - kS) * t2.y;

		// Add contribution
		color += (specular + kD * diffuse) * light.Radiance * light.Intensity;
		//result += vec3(0.5, 0.5, 0.5);
    }

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 
    // Final color output
    finalColor = vec4(color, 1.0);
}