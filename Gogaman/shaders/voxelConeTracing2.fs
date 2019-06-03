//Cones from http://neilsloane.com/sphdesigns/

#version 450 core

#define PI      3.14159265359f
#define SQRT2   1.41421356237f
#define EPSILON 0.000001f

//Multiple of voxel grid size
#define CONE_TRACE_MAX_DISTANCE 2.0f
//Set higher for better performance but lower quality
#define CONE_TRACE_STEP_SIZE 0.5f
//Number of diffuse cones to trace (16 | 32)
#define NUM_DIFFUSE_CONES 16
//Offset origin position of diffuse cones
#define DIFFUSE_CONE_OFFSET 3.0f
//Offset origin position of specular cone
#define SPECULAR_CONE_OFFSET 2.0f
//Minimum specular cone aperture (higher for better performance but lower quality mirror reflections)
#define MIN_SPECULAR_CONE_APERTURE 0.05f

in vec2 texCoordsFrag;

layout(location = 0) out vec4 diffuseColor;
layout(location = 1) out vec4 specularColor;

struct PointLight
{
	vec3 position;
	vec3 color;
};

const int maxPointLights = 3;
uniform PointLight pointLights[maxPointLights];
uniform int        numLights;
uniform int        renderMode;
//Voxel grid size in meters
uniform float      voxelGridSize;
uniform float      voxelGridSizeInverse;
//Size of one voxel in meters (voxelGridSize / voxelResolution)
uniform float      voxelWorldSize;
//World space voxel grid center position
uniform vec3       voxelGridPos;
//World space camera position
uniform vec3       cameraPos;
//Texture coordinate offset for temporal resolve
uniform vec2       coordJitter;
//G-buffer textures
uniform sampler2D gPositionMetalness;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoEmissivityRoughness;
//Voxel texture
uniform sampler3D voxelTotalRadiance;

uniform bool debug;

//Inverse square falloff attenuation
float attenuate(float dist)
{ return 1.0f / (dist * dist); }
float chrominanceEdgeDirectedReconstructionFilter(vec2 a0, vec2 a1, vec2 a2, vec2 a3, vec2 a4);
vec3  decodeAlbedo(sampler2D albedoTexture);
vec2  signNotZero(vec2 v)
{ return vec2((v.x >= 0.0f) ? 1.0f : -1.0f, (v.y >= 0.0f) ? 1.0f : -1.0f); }
vec3 decodeSignedOctahedronNormal(vec2 e);
bool  isInVoxelVolume(vec3 worldPos);
vec4  readVoxel(vec3 worldPos, float mipLevel);
vec3  traceCone(vec3 coneDirection, float coneAperture, float offset);
float traceShadowCone(vec3 coneDirection, float coneAperture, float maxTracingDistance);
vec3  ComputeIndirectDiffuse();
vec3  ComputeIndirectSpecular();
float ComputeVisibility();
float computePointLightVisibility(PointLight light);

const mat3 YCOCGtoRGB = mat3(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 1.0f, -1.0f);

#if NUM_DIFFUSE_CONES == 16
	const vec3 diffuseConeDirections[16] = { vec3(0.57735f, 0.57735f, 0.57735f), vec3(0.57735f, -0.57735f, -0.57735f), vec3(-0.57735f, 0.57735f, -0.57735f), vec3(-0.57735f, -0.57735f, 0.57735f), vec3(-0.903007f, -0.182696f, -0.388844f), vec3(-0.903007f, 0.182696f, 0.388844f), vec3(0.903007f, -0.182696f, 0.388844f), vec3(0.903007f, 0.182696f, -0.388844f), vec3(-0.388844f, -0.903007f, -0.182696f), vec3(0.388844f, -0.903007f, 0.182696f), vec3(0.388844f, 0.903007f, -0.182696f), vec3(-0.388844f, 0.903007f, 0.182696f), vec3(-0.182696f, -0.388844f, -0.903007f), vec3(0.182696f, 0.388844f, -0.903007f), vec3(-0.182696f, 0.388844f, 0.903007f), vec3(0.182696f, -0.388844f, 0.903007f) };	
	#define DIFFUSE_CONE_APERTURE 0.872665f
#else
	const vec3 diffuseConeDirections[32] = { vec3(0.898904f, 0.435512f, 0.0479745f), vec3(0.898904f, -0.435512f, -0.0479745f), vec3(0.898904f, 0.0479745f, -0.435512f), vec3(0.898904f, -0.0479745f, 0.435512f), vec3(-0.898904f, 0.435512f, -0.0479745f), vec3(-0.898904f, -0.435512f, 0.0479745f), vec3(-0.898904f, 0.0479745f, 0.435512f), vec3(-0.898904f, -0.0479745f, -0.435512f), vec3(0.0479745f, 0.898904f, 0.435512f), vec3(-0.0479745f, 0.898904f, -0.435512f), vec3(-0.435512f, 0.898904f, 0.0479745f), vec3(0.435512f, 0.898904f, -0.0479745f), vec3(-0.0479745f, -0.898904f, 0.435512f), vec3(0.0479745f, -0.898904f, -0.435512f), vec3(0.435512f, -0.898904f, 0.0479745f), vec3(-0.435512f, -0.898904f, -0.0479745f), vec3(0.435512f, 0.0479745f, 0.898904f), vec3(-0.435512f, -0.0479745f, 0.898904f), vec3(0.0479745f, -0.435512f, 0.898904f), vec3(-0.0479745f, 0.435512f, 0.898904f), vec3(0.435512f, -0.0479745f, -0.898904f), vec3(-0.435512f, 0.0479745f, -0.898904f), vec3(0.0479745f, 0.435512f, -0.898904f), vec3(-0.0479745f, -0.435512f, -0.898904f), vec3(0.57735f, 0.57735f, 0.57735f), vec3(0.57735f, 0.57735f, -0.57735f), vec3(0.57735f, -0.57735f, 0.57735f), vec3(0.57735f, -0.57735f, -0.57735f), vec3(-0.57735f, 0.57735f, 0.57735f), vec3(-0.57735f, 0.57735f, -0.57735f), vec3(-0.57735f, -0.57735f, 0.57735f), vec3(-0.57735f, -0.57735f, -0.57735f) };
	#define DIFFUSE_CONE_APERTURE 0.628319f
#endif

//Unpack world space position and metalness
vec4 positionMetalness = texture(gPositionMetalness, texCoordsFrag + coordJitter);
vec3 position          = positionMetalness.xyz;
float metalness        = positionMetalness.a;
//Unpack normal
vec3 normal            = decodeSignedOctahedronNormal(texture(gNormal, texCoordsFrag + coordJitter).xy);
//Unpack albedo, emissivity, and roughness
vec4 albedoEmissivityRoughness = texture(gAlbedoEmissivityRoughness, texCoordsFrag + coordJitter);
vec3 albedo            = decodeAlbedo(gAlbedoEmissivityRoughness);
float emissivity       = albedoEmissivityRoughness.b;
float roughness        = albedoEmissivityRoughness.a;
float IOR              = 1.33f;
float transparency     = 0.0f;

vec3 viewDir = normalize(cameraPos - position);

void main()
{
	if(dot(albedo, vec3(1.0f)) < EPSILON)
		discard;
	
	diffuseColor  = vec4(ComputeIndirectDiffuse(),  1.0f);
	specularColor = vec4(ComputeIndirectSpecular(), 1.0f);
}

float chrominanceEdgeDirectedReconstructionFilter(vec2 a0, vec2 a1, vec2 a2, vec2 a3, vec2 a4)
{
	const float threshold = 30.0f / 255.0f;

	vec4 luminance = vec4(a1.x, a2.x , a3.x, a4.x);
	vec4 w         = 1.0f - step(threshold, abs(luminance - a0.x));
	float W        = w.x + w.y + w.z + w.w;
	//float W = dot(W, vec4(1.0f));

	//Special case when all weights are zero
	w.x = (W == 0.0f) ? 1.0f : w.x;
	W   = (W == 0.0f) ? 1.0f : W;

	return (w.x * a1.y + w.y * a2.y + w.z * a3.y + w.w * a4.y) / W;
}

vec3 decodeAlbedo(sampler2D albedoTexture)
{
	vec2 textureDimensions = textureSize(albedoTexture, 0);
	vec2 coords = gl_FragCoord.xy / textureDimensions;
	vec3 decodedAlbedo = texture(albedoTexture, coords).rgb;

	//Reconstruction
	vec2 a0 = texture(albedoTexture, coords + vec2(1.0f / textureDimensions.x,  0.0f)).rg;
	vec2 a1 = texture(albedoTexture, coords - vec2(1.0f / textureDimensions.x,  0.0f)).rg;
	vec2 a2 = texture(albedoTexture, coords + vec2(0.0f, 1.0f / textureDimensions.y)).rg;
	vec2 a3 = texture(albedoTexture, coords - vec2(0.0f, 1.0f / textureDimensions.y)).rg;

	float chrominance = chrominanceEdgeDirectedReconstructionFilter(decodedAlbedo.rg, a0, a1, a2, a3);

	decodedAlbedo.b = chrominance;
	decodedAlbedo   = ((ivec2(gl_FragCoord).x & 1) == (ivec2(gl_FragCoord).y & 1)) ? decodedAlbedo.rbg : decodedAlbedo.rgb;
	decodedAlbedo   = YCOCGtoRGB * decodedAlbedo;
	return decodedAlbedo;
}

vec3 decodeSignedOctahedronNormal(vec2 e)
{
	vec3 v = vec3(e.xy, 1.0f - abs(e.x) - abs(e.y));
	if(v.z < 0.0f)
		v.xy = (1.0f - abs(v.yx)) * signNotZero(v.xy);
	return normalize(v);
}

bool isInVoxelVolume(vec3 worldPos)
{
	vec3 voxelPos = vec3((worldPos - voxelGridPos) * voxelGridSizeInverse);
	return !(any(greaterThan(abs(voxelPos), vec3(1.0f))));
}

vec4 readVoxel(vec3 worldPos, float mipLevel)
{
	//World to voxel space
	vec3 voxelPos = vec3((worldPos - voxelGridPos) * voxelGridSizeInverse);
	//[-1, 1] to [0, 1]
	voxelPos = (voxelPos * 0.5f) + vec3(0.5f);
	return textureLod(voxelTotalRadiance, voxelPos, mipLevel);
}

vec3 traceCone(vec3 coneDirection, float coneAperture, float offset)
{
	vec4 color     = vec4(0.0f);
	float coneDiameterCoefficient = 2.0f * tan(coneAperture * 0.5f);
	//Offset cone to prevent self-sampling (square root of 2 is voxel diagonal half-extent)
	float distance = voxelWorldSize * offset;
	vec3 startPos  = position + normal * distance * 2.0f * SQRT2;
	float maxDist  = CONE_TRACE_MAX_DISTANCE * voxelGridSize;

	while((distance < maxDist) && (color.a < 1.0f))
	{
		vec3  samplePosition = startPos + coneDirection * distance;
		float coneDiameter   = max(coneDiameterCoefficient * distance, voxelWorldSize);
		float mipLevel       = log2(coneDiameter / voxelWorldSize);
		
		//Ensure cone is within voxel volume and last mip level has not been sampled
		if((!isInVoxelVolume(samplePosition)) || (mipLevel > textureQueryLevels(voxelTotalRadiance)))
			break;
		
		vec4 sampleColor = readVoxel(samplePosition, mipLevel);

		//Front to back blending to avoid artifacts
		color += (1.0f - color.a) * sampleColor;

		distance += coneDiameter * CONE_TRACE_STEP_SIZE;
	}

	return color.rgb;
}

vec3 ComputeIndirectDiffuse()
{
	vec3 Lo = vec3(0.0f);
	for(int i = 0; i < NUM_DIFFUSE_CONES; i++)
	{
		if(dot(normal, diffuseConeDirections[i]) < 0.0f)
			continue;
		
		Lo += traceCone(diffuseConeDirections[i], DIFFUSE_CONE_APERTURE, DIFFUSE_CONE_OFFSET);
	}
	
	Lo /= NUM_DIFFUSE_CONES * 0.5f;
	//Lo *= 1.0f / (NUM_DIFFUSE_CONES * 0.5f);
	return Lo;
}

vec3 ComputeIndirectSpecular()
{
	vec3  coneDirection = normalize(reflect(-viewDir, normal));
	float coneAperture  = max(tan(0.0003474660443456835f + (roughness * (1.3331290497744692f - (roughness * 0.5040552688878546f)))), MIN_SPECULAR_CONE_APERTURE);
	coneAperture = 0.002f;
	return traceCone(coneDirection, coneAperture, SPECULAR_CONE_OFFSET);
}