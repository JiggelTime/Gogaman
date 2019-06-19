#version 450 core

#define PI      3.14159265359f
#define SQRT2   1.41421356237f
#define EPSILON 0.000001f

#define MAX_POINT_LIGHTS 3
//DISNEY or LAMBERT, Disney is higher quality but slower
#define DIFFUSE_BRDF DISNEY
//Lower quality but faster
#define USE_FAST_VISIBILITY_SMITH_GGX 0
//Multiple of voxel grid size
#define CONE_TRACE_MAX_DISTANCE 2.0f
//Higher is lower quality but faster
#define CONE_TRACE_STEP_SIZE 0.5f

layout(location = 0) out vec4 FragColor;

in vec2 texCoordsFrag;

struct PointLight 
{
	vec3  position;
	vec3  color;
	float coneAperture;
};

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int        numLights;
//World space camera position
uniform vec3       cameraPos;
//Voxel grid size in meters
uniform float      voxelGridSize;
uniform float      voxelGridSizeInverse;
//Size of one voxel in meters (voxelGridSize / voxelResolution)
uniform float      voxelWorldSize;
//World space voxel grid center position
uniform vec3       voxelGridPos;
uniform sampler2D gPositionMetalness;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoEmissivityRoughness;
uniform sampler2D BRDF_LUT;
uniform sampler2D coneTracedDiffuse;
uniform sampler2D coneTracedSpecular;
uniform sampler3D voxelTexture;

uniform int  renderMode;
uniform bool debug;
uniform bool debug2;
/*
vec3 worldPosFromDepth(float depth)
{
    vec4 p = vec4(In.texCoords, depth, 1.0);
    p.xyz = p.xyz * 2.0 - 1.0;
    p = u_viewProjInv * p;
    return p.xyz / p.w;
}*/
float chrominanceEdgeDirectedReconstructionFilter(vec2 a0, vec2 a1, vec2 a2, vec2 a3, vec2 a4);
vec3  DecodeAlbedo(sampler2D albedoTexture);
vec2  SignNotZero(vec2 v) { return vec2((v.x >= 0.0f) ? 1.0f : -1.0f, (v.y >= 0.0f) ? 1.0f : -1.0f); }
vec3  DecodeSignedOctahedronNormal(vec2 e);
//Inverse square falloff attenuation
float Attenuation(float dist) { return 1.0f / (dist * dist); }
vec3  ComputePointLight(PointLight light);
vec3  ComputeDirectRadiance();
vec3  ComputeIndirectRadiance();
float DistributionGGX(float NdotH, float alpha2);
float VisibilitySmithGGX(float NdotL, float NdotV, float alpha2);
float FastVisibilitySmithGGX(float NdotL, float NdotV, float alpha);
vec3  FresnelSchlick(float cosTheta, vec3 f0, float f90);
vec3  FresnelSchlick(float cosTheta, vec3 f0);
vec3  FresnelSchlickRoughness(float cosTheta, vec3 f0, float alpha);
float DiffuseBurley(float NdotV, float NdotL, float LdotH, float linearRoughness);
bool  IsInVoxelVolume(vec3 worldPos) { return !(any(greaterThan(abs(vec3((worldPos - voxelGridPos) * voxelGridSizeInverse)), vec3(1.0f)))); }
vec4  ReadVoxel(vec3 worldPos, float mipLevel);
float TraceShadowCone(vec3 direction, float aperture, float maxTracingDistance);

const mat3 YCOCGtoRGB = mat3(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 1.0f, -1.0f);

//Retrieve world space position and metalness
vec4  positionMetalness = texture(gPositionMetalness, texCoordsFrag);
vec3  position          = positionMetalness.xyz;
float metalness         = positionMetalness.a;
//Retrieve normal
vec3  normal            = DecodeSignedOctahedronNormal(texture(gNormal, texCoordsFrag).xy);
//Retrieve albedo, emissivity, and roughness
vec4  albedoEmissivityRoughness = texture(gAlbedoEmissivityRoughness, texCoordsFrag).xyzw;
vec3  albedo            = DecodeAlbedo(gAlbedoEmissivityRoughness);
float emissivity        = albedoEmissivityRoughness.z;
float roughness         = albedoEmissivityRoughness.w;
//Remap roughness by squaring
float linearRoughness   = roughness * roughness;
//Retrieve cone traced diffuse
vec3  indirectDiffuse   = texture(coneTracedDiffuse, texCoordsFrag).rgb;
//Retrieve cone traced specular
vec3  indirectSpecular  = texture(coneTracedSpecular, texCoordsFrag).rgb;

vec3 viewDir = normalize(cameraPos - position);
//Assume dielectric surfaces have a constant f0 of 0.04
vec3 f0 = mix(vec3(0.04f), albedo, metalness);

void main()
{
	vec3 color = vec3(0.0f, 0.0f, 0.0f);

	vec3 indirect = ComputeIndirectRadiance();
	vec3 direct   = ComputeDirectRadiance();

	//Total radiance
	if(renderMode == 0)
		color = direct + indirect;

	//Debug visualization
		//Direct lighting
		else if(renderMode == 1)
			color = direct;
		//Indirect lighting
		else if(renderMode == 2)
			color = indirect;
		//Indirect diffuse
		else if(renderMode == 3)
			color = indirectDiffuse;
		//Indirect specular
		else if(renderMode == 4)
			color = indirectSpecular;
		//Emissivity
		else if(renderMode == 5)
			color = vec3(emissivity);
		//Albedo
		else if(renderMode == 6)
			color = albedo;
		//Normal
		else if(renderMode == 7)
			color = abs(normal);
		//Roughness
		else
			color = vec3(linearRoughness);

	FragColor = vec4(color, 1.0f);
}

float chrominanceEdgeDirectedReconstructionFilter(vec2 a0, vec2 a1, vec2 a2, vec2 a3, vec2 a4)
{
	const float threshold = 30.0f / 255.0f;

	vec4 luminance = vec4(a1.x, a2.x , a3.x, a4.x);
	vec4 w         = 1.0f - step(threshold, abs(luminance - a0.x));
	float W        = w.x + w.y + w.z + w.w;
	//float W = dot(W, vec4(1.0f));

	//Special case where all the weights are zero
	w.x = (W == 0.0f) ? 1.0f : w.x;
	W   = (W == 0.0f) ? 1.0f : W;

	return (w.x * a1.y + w.y * a2.y + w.z * a3.y + w.w * a4.y) / W;
}

vec3 DecodeAlbedo(sampler2D albedoTexture)
{
	vec2 textureDimensions = textureSize(albedoTexture, 0);
	vec2 texelSize         = 1.0f / textureDimensions;
	vec2 texCoords         = gl_FragCoord.xy / vec2(textureDimensions.x, textureDimensions.y);
	vec3 decodedAlbedo     = texture(albedoTexture, texCoords).rgb;

	//Co/Cg reconstruction
	vec2 a0 = texture(albedoTexture, texCoords + vec2(texelSize.x, 0.0f)).rg;
	vec2 a1 = texture(albedoTexture, texCoords - vec2(texelSize.x, 0.0f)).rg;
	vec2 a2 = texture(albedoTexture, texCoords + vec2(0.0f, texelSize.y)).rg;
	vec2 a3 = texture(albedoTexture, texCoords - vec2(0.0f, texelSize.y)).rg;
	float chrominance = chrominanceEdgeDirectedReconstructionFilter(decodedAlbedo.rg, a0, a1, a2, a3);

	decodedAlbedo.z = chrominance;
	decodedAlbedo   = ((ivec2(gl_FragCoord).x & 1) == (ivec2(gl_FragCoord).y & 1)) ? decodedAlbedo.rbg : decodedAlbedo.rgb;
	decodedAlbedo   = YCOCGtoRGB * decodedAlbedo;
	return decodedAlbedo;
}

vec3 DecodeSignedOctahedronNormal(vec2 e)
{
	vec3 v = vec3(e.xy, 1.0f - abs(e.x) - abs(e.y));
	if(v.z < 0.0f)
		v.xy = (1.0f - abs(v.yx)) * SignNotZero(v.xy);
	return normalize(v);
}

vec3 ComputePointLight(PointLight light)
{
	vec3  Wi    = light.position - position;
	float Ld    = length(Wi);
	Wi          = normalize(Wi);
	vec3  Li    = light.color * Attenuation(Ld);
	vec3  H     = normalize(viewDir + Wi);
	float NdotV = abs(dot(normal, viewDir)) + EPSILON;
	float LdotH = clamp(dot(Wi, H), 0.0f, 1.0f);
	float NdotH = clamp(dot(normal, H), 0.0f, 1.0f);
	float NdotL = clamp(dot(normal, Wi), 0.0f, 1.0f);

	float alpha2 = linearRoughness * linearRoughness;

	//Cook-Torrance specular BRDF
	float NDF = DistributionGGX(NdotH, alpha2);
	#if USE_FAST_VISIBILITY_SMITH_GGX
		float V = FastVisibilitySmithGGX(NdotV, NdotL, linearRoughness);
	#else
		float V = VisibilitySmithGGX(NdotV, NdotL, alpha2);
	#endif
	vec3 F  = FresnelSchlick(LdotH, f0);
	vec3 Fr = NDF * V * F;

	//Disney or Lambert diffuse BRDF
	#if DIFFUSE_BRDF == DISNEY
		vec3 Fd = albedo * DiffuseBurley(NdotV, NdotL, LdotH, linearRoughness);
	#elif DIFFUSE_BRDF == LAMBERT
		vec3 Fd = albedo / PI;
	#else
		vec3 Fd = vec3(0.0f);
	#endif

	float Vis = TraceShadowCone(Wi, light.coneAperture, Ld);

	//Ensure energy conservation (Fr is already multiplied by F)
	vec3 Kd = (1.0f - F) * (1.0f - metalness);
	vec3 Lo = (Fd * Kd + Fr) * Li * Vis * NdotL;
	//if(debug2) Lo = vec3(Vis);
	return Lo;
}

vec3 ComputeDirectRadiance()
{
	vec3 Lo = vec3(0.0f);
	if(any(greaterThan(albedo.rgb, vec3(EPSILON))))
	{
		for(int i = 0; i < min(numLights, MAX_POINT_LIGHTS); i++) 
			Lo += ComputePointLight(pointLights[i]);
	}

	Lo += emissivity * albedo;
	return Lo;
}

vec3 ComputeIndirectRadiance()
{
	vec2 precomputedBRDF = texture(BRDF_LUT, vec2(max(dot(normal, viewDir), 0.0f), linearRoughness)).rg;

	vec3 F  = FresnelSchlickRoughness(max(dot(normal, viewDir), 0.0f), f0, roughness);
	vec3 Fd = indirectDiffuse * albedo / PI;
	//TODO: Fr is too intense for rough surfaces
	vec3 Fr = indirectSpecular * (F * precomputedBRDF.x + precomputedBRDF.y);
	//Ensure energy conservation (Fr is already multiplied by F)
	vec3 Kd = (1.0f - F) * (1.0f - metalness);
	//Outgoing radiance
	vec3 Lo = Fd * Kd + Fr;
	return Lo;
}

//Normal distribution function (Trowbridge-Reitz GGX)
float DistributionGGX(float NdotH, float alpha2)
{
	float f = (NdotH * alpha2 - NdotH) * NdotH + 1.0f;
	return alpha2 / (f * f * PI);
}

//Height-correlated Smith-GGX visibility function
float VisibilitySmithGGX(float NdotL, float NdotV, float alpha2)
{
	float SchlickGGX_V = NdotL * sqrt((-NdotV * alpha2 + NdotV) * NdotV + alpha2);
	float SchlickGGX_L = NdotV * sqrt((-NdotL * alpha2 + NdotL) * NdotL + alpha2);
	return 0.5f / (SchlickGGX_V + SchlickGGX_L);
}

//Approximated height-correlated Smith-GGX visibility function
float FastVisibilitySmithGGX(float NdotL, float NdotV, float alpha)
{
	return 0.5f / mix(2.0f * NdotL * NdotV, NdotL + NdotV, alpha);
}

//Schlick Fresnel function
vec3 FresnelSchlick(float cosTheta, vec3 f0, float f90)
{
	//float factor = pow(1.0f - cosTheta, 5);
	float factor  = 1.0f - cosTheta;
	float factor2 = factor * factor;
	factor        = factor2 * factor2 * factor;
	return f0 + (vec3(f90) - f0) * factor;
}

//Schlick Fresnel function with f90 = 1
vec3 FresnelSchlick(float cosTheta, vec3 f0)
{
	//float factor = pow(1.0f - cosTheta, 5);
	float factor  = 1.0f - cosTheta;
	float factor2 = factor * factor;
	factor        = factor2 * factor2 * factor;
	return factor + f0 * (1.0f - factor);
}

//https://seblagarde.wordpress.com/2011/08/17/hello-world/
//Schlick Fresnel function with injected roughness term
vec3 FresnelSchlickRoughness(float cosTheta, vec3 f0, float alpha)
{
	//float factor = pow(1.0f - cosTheta, 5);
	float factor  = 1.0f - cosTheta;
	float factor2 = factor * factor;
	factor        = factor2 * factor2 * factor;
	return f0 + (max(vec3(1.0f - alpha), f0) - f0) * factor;
}

//Burley, Brent, "Physically-Based Shading at Disney", part of "Practical Physically Based Shading in Film and Game Production", SIGGRAPH 2012 Course Notes. http://blog.selfshadow.com/publications/s2012-shading-course/
//Energy-renormalized Disney diffuse BRDF
float DiffuseBurley(float NdotV, float NdotL, float LdotH, float linearRoughness)
{
	float energyBias   = mix(0.0f, 0.5f, linearRoughness);
	float energyFactor = mix(1.0f, 1.0f / 1.51f, linearRoughness);
	float f90          = energyBias + 2.0f * LdotH * LdotH * linearRoughness;
	float lightScatter = FresnelSchlick(NdotL, vec3(1.0f), f90).x;
	float viewScatter  = FresnelSchlick(NdotV, vec3(1.0f), f90).x;
	return lightScatter * viewScatter * energyFactor / PI;
}

vec4 ReadVoxel(vec3 worldPos, float mipLevel)
{
	//World to voxel space
	vec3 voxelPos = vec3((worldPos - voxelGridPos) * voxelGridSizeInverse);
	//[-1, 1] to [0, 1]
	voxelPos = (voxelPos * 0.5f) + vec3(0.5f);
	return textureLod(voxelTexture, voxelPos, mipLevel);
}

float TraceShadowCone(vec3 direction, float aperture, float maxTracingDistance)
{
	float alpha    = 0.0f;
	//Offset cone to prevent self-sampling (square root of 2 is diagonal half-extent)
	float distance = SQRT2 * voxelWorldSize * 2.0f;
	float maxDist  = min(CONE_TRACE_MAX_DISTANCE * voxelGridSize, maxTracingDistance);
	
	while((distance < maxDist) && (alpha < 1.0f))
	{
		vec3  samplePosition = position + direction * distance;
		float diameter       = max(2.0f * aperture * distance, voxelWorldSize);
		float mipLevel       = log2(diameter / voxelWorldSize);

		if(!IsInVoxelVolume(samplePosition))
			break;

		float sampleAlpha = ReadVoxel(samplePosition, mipLevel).a;

		//Front to back blending to avoid artifacts
		alpha    += (1.0f - alpha) * sampleAlpha;
		distance += diameter * CONE_TRACE_STEP_SIZE;
	}

	return clamp(1.0f - alpha, 0.0f, 1.0f);
}