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
    vec4 PositionHLS; // Homogeneous Light Space
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

layout(location = 0) out vec4 FinalColor;

in Varyings
{
    vec2 TexCoord;
    vec3 NormalWS;
    vec3 PositionWS;
    mat3 TBN;
    vec4 PositionHLS;
} fs_in;

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

uniform sampler2D u_ShadowMap;

uniform sampler2D u_OcclusionMap;

const float PI = 3.141592;

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
    float ShadowBias;
    float ShadowNormalBias;

    vec2 Resolution;
} u_Scene;

struct PointLight
{
    vec3 PositionWS;
    vec3 Radiance;
    float Intensity;
    float MinRange;
    float Range;
};

layout(std140, binding = 1) uniform PointLightData
{
    uint LightCount;
    PointLight Lights[256];
} u_PointLights;

struct SpotLight
{
    vec3 PositionWS;
    vec3 Direction;
    vec3 Radiance;
    float Intensity;
    float MinRange;
    float Range;
    float MinAngle;
    float Angle;
};

layout(std140, binding = 2) uniform SpotLightData
{
    uint LightCount;
    SpotLight Lights[256];
} u_SpotLights;

struct AreaLight
{
    vec3 PointsWS[4];
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

    float z = vsum.z / len;
    if (behind)
        z = -z;

    vec2 uv = vec2(z * 0.5f + 0.5f, len); // range [0, 1]
    uv = uv * LUT_SCALE + LUT_BIAS;

    // 通过参数获得几何衰减系数
    float scale = texture(u_LTC2, uv).w;

    vec3 Lo_i = vec3(0.0);
    // 计算每个区域光源点对该点的贡献
    for (int i = 0; i < 4; ++i) {
        vec3 lightDir = normalize(points[i] - P);
        float dotProduct = dot(lightDir, N);
        
        // 仅在光源点朝向表面时才考虑贡献
        if (dotProduct > 0.0) {
            float contribution = len * scale * dotProduct; // 根据点积调整贡献
            Lo_i += vec3(contribution, contribution, contribution);
        }
    }

    // 输出
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

float ShadowCalculation(vec4 PositionHLS, vec3 lightDir)
{
    vec3 projCoords = PositionHLS.xyz / PositionHLS.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;

    vec3 normal = normalize(fs_in.NormalWS);
    float bias = max(u_Scene.ShadowNormalBias * (1.0 - dot(normal, lightDir)), u_Scene.ShadowBias);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;
}

vec3 MultiBounceAO(float ao, vec3 albedo)
{
    vec3 a = 2.0404 * albedo - 0.3324;
    vec3 b = -4.7951 * albedo + 0.6417;
    vec3 c = 2.7552 * albedo + 0.6903;
    
    vec3 x = vec3(ao);
    return max(x, ((x * a + b) * x + c) * x);
}

void main()
{
    // Sample textures
    vec4 albedo = texture(u_AlbedoTexture, fs_in.TexCoord) * u_AlbedoColor;
    float metallic = texture(u_MetallicTexture, fs_in.TexCoord).r * u_Metallic;
    float roughness = texture(u_RoughnessTexture, fs_in.TexCoord).r * u_Roughness;
    float ao = texture(u_OcclusionMap, gl_FragCoord.xy / u_Scene.Resolution).r;
    // Transform normal from tangent space to world space
    vec3 normal = normalize(fs_in.TBN * (texture(u_NormalTexture, fs_in.TexCoord).xyz * 2.0 - 1.0) * u_NormalScale);
    
    // Calculate view direction
    vec3 viewDir = normalize(u_Scene.CameraPosition - fs_in.PositionWS);
    
    // Initialize color
    vec3 color = vec3(0.0);
    
    // Ambient lighting
    color += IBL(normal, viewDir, albedo, metallic, roughness) * u_Scene.EnvironmentIntensity;

    // Directional lights
    for (uint i = 0; i < 4; ++i)
    {
        DirectionalLight light = u_Scene.DirectionalLights[i];
        vec3 lightDir = normalize(-light.Direction);

        float shadow = 0.0;
        if (i == 0){
            shadow = ShadowCalculation(fs_in.PositionHLS, lightDir); 
        }

        color += CookTorranceBRDF(normal, viewDir, lightDir, metallic, roughness, albedo) * light.Radiance * light.Intensity * (1.0 - shadow);
    }

    // Point lights
    for (uint i = 0; i < u_PointLights.LightCount; ++i)
    {
        PointLight light = u_PointLights.Lights[i];
        float distance = length(light.PositionWS - fs_in.PositionWS);
        if (distance > light.Range)
            continue;
        vec3 lightDir = normalize(light.PositionWS - fs_in.PositionWS);
        // Attenuation
        float attenuation = 1 - smoothstep(light.MinRange, light.Range, distance);
        color += CookTorranceBRDF(normal, viewDir, lightDir, metallic, roughness, albedo) * light.Radiance * light.Intensity * attenuation;
    }

    // Spot lights
    for (uint i = 0; i < u_SpotLights.LightCount; ++i)
    {
        SpotLight light = u_SpotLights.Lights[i];
        float distance = length(light.PositionWS - fs_in.PositionWS);
        if (distance > light.Range)
            continue;
        vec3 lightDir = normalize(light.PositionWS - fs_in.PositionWS);
        // Attenuation
        float attenuation = 1 - smoothstep(light.MinRange, light.Range, distance);
        
        // Angle attenuation
        float angleAttenuation = 1 - smoothstep(cos(radians(light.MinAngle)), cos(radians(light.Angle)), dot(-lightDir, normalize(light.Direction)));
        attenuation *= angleAttenuation;
        
        color += CookTorranceBRDF(normal, viewDir, lightDir, metallic, roughness, albedo) * light.Radiance * light.Intensity * attenuation;
    }

    // Area lights
    vec3 N = normal;
    vec3 V = viewDir;
    vec3 P = fs_in.PositionWS;

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
		vec3 diffuse = LTC_Evaluate(N, V, P, mat3(1), light.PointsWS, light.TwoSided);
		vec3 specular = LTC_Evaluate(N, V, P, Minv, light.PointsWS, light.TwoSided);
        diffuse *= albedo.rgb;
		// GGX BRDF shadowing and Fresnel
		// t2.x: shadowedF90 (F90 normally it should be 1.0)
		// t2.y: Smith function for Geometric Attenuation Term, it is dot(V or L, H).
		specular *= kS * t2.x + (1.0f - kS) * t2.y;

		// Add contribution
		color += (specular + kD * diffuse) * light.Radiance * light.Intensity;
    }

    // Final color output
    color *= MultiBounceAO(ao, albedo.rgb);
    FinalColor = vec4(color, 1.0);
}