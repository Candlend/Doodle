/*
 * This shader implements GTAO (Ground Truth Ambient Occlusion)
 * References upon which this is based:
 * - https://www.activision.com/cdn/research/Practical_Real_Time_Strategies_for_Accurate_Indirect_Occlusion_NEW%20VERSION_COLOR.pdf
 * - https://github.com/X0nk/Bliss-Shader
 */
#type vertex
#version 450

layout(location = 0) in vec3 a_PositionOS;

out vec2 v_TexCoord;

void main()
{
	gl_Position = vec4(a_PositionOS, 1.0);
    v_TexCoord = (a_PositionOS.xy + 1.0) / 2.0;
}

#type fragment
#version 450
#extension GL_EXT_gpu_shader4 : enable

layout (location = 0) out vec4 Ao;

in vec2 v_TexCoord;

// 绑定的深度纹理
layout (binding = 0) uniform sampler2D u_GPositionWS;
layout (binding = 1) uniform sampler2D u_GNormalWS;

uniform sampler2D u_GDepth;
uniform mat4 u_Projection;
uniform mat4 u_InverseProjection;
uniform mat4 u_View;
uniform int u_FrameCounter;
uniform sampler2D u_NoiseTexture;
uniform vec2 u_Resolution;
uniform vec2 u_PixelSize;

// Common constants
const float pi           = 3.14159265359;
const float eps          = 1e-6;
const float e            = exp(1.0);
const float tau          = 2.0 * pi;
const float half_pi      = 0.5 * pi;
const float rcp_pi       = 1.0 / pi;
const float degree       = tau / 360.0; // Size of one degree in radians, useful because radians() is not a constant expression on all platforms
const float golden_ratio = 0.5 + 0.5 * sqrt(5.0);
const float golden_angle = tau / golden_ratio / golden_ratio;
const float hand_depth   = 0.56;

#ifndef TAA
  #undef TAA_UPSCALING
#endif

#ifdef TAA_UPSCALING
  #define SCALE_FACTOR 0.75  // render resolution multiplier. below 0.5 not recommended [0.10 0.15 0.20 0.25 0.30 0.35 0.40 0.45 0.50 0.55 0.60 0.65 0.70 0.75 0.80 0.85 0.90 0.95 1.0]

  #define RENDER_SCALE vec2(SCALE_FACTOR, SCALE_FACTOR)
  #define UPSCALING_SHARPNENING 2.0 - SCALE_FACTOR - SCALE_FACTOR
#else
  #define RENDER_SCALE vec2(1.0, 1.0)
  #define UPSCALING_SHARPNENING 0.0
#endif

#if defined TAA && defined TAA_UPSCALING
const float taau_render_scale = RENDER_SCALE.x;
#else
const float taau_render_scale = 1.0;
#endif

// Helper functions

#define rcp(x) (1.0 / (x))
#define clamp01(x) clamp(x, 0.0, 1.0) // free on operation output
#define max0(x) max(x, 0.0)
#define min1(x) min(x, 1.0)

float sqr(float x) { return x * x; }
vec2  sqr(vec2  v) { return v * v; }
vec3  sqr(vec3  v) { return v * v; }
vec4  sqr(vec4  v) { return v * v; }

float cube(float x) { return x * x * x; }

float max_of(vec2 v) { return max(v.x, v.y); }
float max_of(vec3 v) { return max(v.x, max(v.y, v.z)); }
float max_of(vec4 v) { return max(v.x, max(v.y, max(v.z, v.w))); }
float min_of(vec2 v) { return min(v.x, v.y); }
float min_of(vec3 v) { return min(v.x, min(v.y, v.z)); }
float min_of(vec4 v) { return min(v.x, min(v.y, min(v.z, v.w))); }

float length_squared(vec2 v) { return dot(v, v); }
float length_squared(vec3 v) { return dot(v, v); }

vec2 normalize_safe(vec2 v) { return v == vec2(0.0) ? v : normalize(v); }
vec3 normalize_safe(vec3 v) { return v == vec3(0.0) ? v : normalize(v); }

float rcp_length(vec2 v) { return inversesqrt(dot(v, v)); }
float rcp_length(vec3 v) { return inversesqrt(dot(v, v)); }

float fast_acos(float x) {
	const float C0 = 1.57018;
	const float C1 = -0.201877;
	const float C2 = 0.0464619;

	float res = (C2 * abs(x) + C1) * abs(x) + C0; // p(x)
	res *= sqrt(1.0 - abs(x));

	return x >= 0 ? res : pi - res; // Undo range reduction
}
vec2 fast_acos(vec2 v) { return vec2(fast_acos(v.x), fast_acos(v.y)); }

float linear_step(float edge0, float edge1, float x) {
	return clamp01((x - edge0) / (edge1 - edge0));
}

vec2 linear_step(vec2 edge0, vec2 edge1, vec2 x) {
	return clamp01((x - edge0) / (edge1 - edge0));
}

vec4 project(mat4 m, vec3 pos) {
    return vec4(m[0].x, m[1].y, m[2].zw) * pos.xyzz + m[3];
}
vec3 project_and_divide(mat4 m, vec3 pos) {
    vec4 homogenous = project(m, pos);
    return homogenous.xyz / homogenous.w;
}

vec3 screen_to_view_space(vec3 screen_pos, bool handle_jitter) {
	vec3 ndc_pos = 2.0 * screen_pos - 1.0;

	return project_and_divide(u_InverseProjection, ndc_pos);
}

vec3 view_to_screen_space(vec3 view_pos, bool handle_jitter) {
	vec3 ndc_pos = project_and_divide(u_Projection, view_pos);


	return ndc_pos * 0.5 + 0.5;
}


// ---------------------
//   ambient occlusion
// ---------------------

uniform int u_Slices = 2;
uniform int u_HorizonSteps = 3;
uniform float u_Radius = 2.0;
uniform float u_FalloffStart = 0.75;

float integrate_arc(vec2 h, float n, float cos_n) {
	vec2 tmp = cos_n + 2.0 * h * sin(n) - cos(2.0 * h - n);
	return 0.25 * (tmp.x + tmp.y);
}

float calculate_maximum_horizon_angle(
	vec3 view_slice_dir,
	vec3 viewer_dir,
	vec3 screen_pos,
	vec3 view_pos,
	float dither
) {
	const float step_size = u_Radius * rcp(float(u_HorizonSteps));

	float max_cos_theta = -1.0;

	vec2 ray_step = (view_to_screen_space(view_pos + view_slice_dir * step_size, true) - screen_pos).xy;
	vec2 ray_pos = screen_pos.xy + ray_step * (dither + max_of(u_PixelSize) * rcp_length(ray_step));


	for (int i = 0; i < u_HorizonSteps; ++i, ray_pos += ray_step) {
		float depth = texelFetch2D(u_GDepth, ivec2(clamp(ray_pos,0.0,1.0) * u_Resolution * taau_render_scale - 0.5), 0).x;

		if (depth == 1.0 || depth < hand_depth || depth == screen_pos.z) continue;

		vec3 offset = screen_to_view_space(vec3(ray_pos, depth), true) - view_pos;

		float len_sq = length_squared(offset);
		float norm = inversesqrt(len_sq);

		float distance_falloff = linear_step(u_FalloffStart * u_Radius, u_Radius, len_sq * norm);

		float cos_theta = dot(viewer_dir, offset) * norm;
		      cos_theta = mix(cos_theta, -1.0, distance_falloff);

		max_cos_theta = max(cos_theta, max_cos_theta);
	}

	return fast_acos(clamp(max_cos_theta, -1.0, 1.0));
}

float ambient_occlusion(vec3 screen_pos, vec3 view_pos, vec3 view_normal, vec2 dither) {
	float ao = 0.0;

	// Construct local working space
	vec3 viewer_dir   = normalize(-view_pos);
	vec3 viewer_right = normalize(cross(vec3(0.0, 1.0, 0.0), viewer_dir));
	vec3 viewer_up    = cross(viewer_dir, viewer_right);
	mat3 local_to_view = mat3(viewer_right, viewer_up, viewer_dir);

	for (int i = 0; i < u_Slices; ++i) {
		float slice_angle = (i + dither.x) * (pi / float(u_Slices));

		vec3 slice_dir = vec3(cos(slice_angle), sin(slice_angle), 0.0);
		vec3 view_slice_dir = local_to_view * slice_dir;

		vec3 ortho_dir = slice_dir - dot(slice_dir, viewer_dir) * viewer_dir;
		vec3 axis = cross(slice_dir, viewer_dir);

		vec3 projected_normal = view_normal - axis * dot(view_normal, axis);

		float len_sq = dot(projected_normal, projected_normal);
		float norm = inversesqrt(len_sq);

		float sgn_gamma = sign(dot(ortho_dir, projected_normal));
		float cos_gamma = clamp01(dot(projected_normal, viewer_dir) * norm);
		float gamma = sgn_gamma * fast_acos(cos_gamma);

		vec2 max_horizon_angles;
		max_horizon_angles.x = calculate_maximum_horizon_angle(-view_slice_dir, viewer_dir, screen_pos, view_pos, dither.y);
		max_horizon_angles.y = calculate_maximum_horizon_angle( view_slice_dir, viewer_dir, screen_pos, view_pos, dither.y);

		max_horizon_angles = gamma + clamp(vec2(-1.0, 1.0) * max_horizon_angles - gamma, -half_pi, half_pi) ;


		ao += integrate_arc(max_horizon_angles, gamma, cos_gamma) * len_sq * norm  ;
	}
	ao *= rcp(float(u_Slices));
	return ao*(ao*0.5+0.5);
}

vec4 blueNoise(vec2 coord){
  return texelFetch2D(u_NoiseTexture, ivec2(coord) % 512 , 0) ;
}

vec2 R2_samples(float n){
	vec2 alpha = vec2(0.75487765, 0.56984026);
	return fract(alpha * n);
}

const vec2[8] offsets = vec2[8](vec2(1./8.,-3./8.),
                                vec2(-1.,3.)/8.,
                                vec2(5.0,1.)/8.,
                                vec2(-3,-5.)/8.,
                                vec2(-5.,5.)/8.,
                                vec2(-7.,-1.)/8.,
                                vec2(3,7.)/8.,
                                vec2(7.,-7.)/8.);

vec3 toScreenSpace(vec3 p) {
	vec4 iProjDiag = vec4(u_InverseProjection[0].x, u_InverseProjection[1].y, u_InverseProjection[2].zw);
    vec3 p3 = p * 2. - 1.;
    vec4 fragposition = iProjDiag * p3.xyzz + u_InverseProjection[3];
    return fragposition.xyz / fragposition.w;
}

void main() {
    vec2 texcoord = gl_FragCoord.xy * u_PixelSize;
    float z = texture2D(u_GDepth, texcoord).x;
    
#ifdef TAA
	vec2 TAA_Offset = offsets[u_FrameCounter % 8];
#else
	vec2 TAA_Offset = vec2(0.0);
#endif

	vec3 screen_pos = vec3(texcoord / RENDER_SCALE - TAA_Offset * u_PixelSize * 0.5, z);
    vec3 view_pos = toScreenSpace(screen_pos);

    vec3 world_normal = texture(u_GNormalWS, texcoord).xyz;
    vec3 view_normal = normalize((u_View * vec4(world_normal, 0.0)).xyz);

#ifdef TAA
    int seed = (u_FrameCounter % 40000) + u_FrameCounter * 2;
#else
	int seed = 0;
#endif
    vec2 r2 = fract(R2_samples(seed) + blueNoise(gl_FragCoord.xy).rg);

    float ao = ambient_occlusion(screen_pos, view_pos, view_normal, r2);

    Ao = vec4(vec3(ao), 1.0);
}
