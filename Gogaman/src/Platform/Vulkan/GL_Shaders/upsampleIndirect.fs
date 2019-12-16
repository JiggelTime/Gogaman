#version 420 core

#define PI 3.14159265359f
#define EPSILON 0.000001f

//Spatial filter settings
#define SPATIAL_KERNEL_RADIUS 2

layout(location = 0) out vec4 FragColorIndirectDiffuse;
layout(location = 1) out vec4 FragColorIndirectSpecular;

in vec2 texCoordsFrag;

uniform float nearPlane;
uniform float farPlane;
uniform int   sampleTextureLod;

uniform sampler2D indirectDiffuseTexture;
uniform sampler2D indirectSpecularTexture;
//Hyperbolic depth
uniform sampler2D depthTexture;
//Downsampled hyperbolic depth
uniform sampler2D depthCoarseTexture;
//Encoded normal
uniform sampler2D normalTexture;
//Downsampled encoded normal
uniform sampler2D normalCoarseTexture;

uniform bool debug;
uniform bool debug2;

vec2  SignNotZero(vec2 v) { return vec2((v.x >= 0.0f) ? 1.0f : -1.0f, (v.y >= 0.0f) ? 1.0f : -1.0f); }
vec3  DecodeSignedOctahedronNormal(vec2 e);
float LinearizeDepth(float depth) { return -farPlane * nearPlane / (depth * (farPlane - nearPlane) - farPlane); }
float Luminance(vec3 linearRGB) { return dot(linearRGB, vec3(0.3f, 0.59f, 0.11f)); }
float TapWeight(vec3 centerN, vec3 sampleN, float centerZ, float sampleZ, vec2 offset, float centerZGradient);
void  SpatialResolve(sampler2D indirectDiffuse, sampler2D indirectSpecular, sampler2D depthTexture, sampler2D depthCoarseTexture, sampler2D normalTexture, sampler2D normalCoarseTexture, out vec4 upsampledIndirectDiffuse, out vec4 upsampledIndirectSpecular);

const mat3 RGBtoYCOCG = mat3(0.25f, 0.5f, -0.25f, 0.5f, 0.0f, 0.5f, 0.25f, -0.5f, -0.25f);
const mat3 YCOCGtoRGB = mat3(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 1.0f, -1.0f);

vec2 resolutionInput  = textureSize(indirectDiffuseTexture,  0);
vec2 texelSizeInput   = 1.0f / resolutionInput;
vec2 resolutionOutput = textureSize(normalTexture, 0);
vec2 texelSizeOutput  = 1.0f / resolutionOutput;

void main()
{
	SpatialResolve(indirectDiffuseTexture, indirectSpecularTexture, depthTexture, depthCoarseTexture, normalTexture, normalCoarseTexture, FragColorIndirectDiffuse, FragColorIndirectSpecular);
	FragColorIndirectSpecular = vec4(0.0f);
}

vec3 DecodeSignedOctahedronNormal(vec2 e)
{
	vec3 v = vec3(e.xy, 1.0f - abs(e.x) - abs(e.y));
	if(v.z < 0.0f) v.xy = (1.0f - abs(v.yx)) * SignNotZero(v.xy);
	return normalize(v);
}

float TapWeight(vec3 centerN, vec3 sampleN, float centerZ, float sampleZ, vec2 offset, float centerZGradient)
{
	//const float sigmaN = 64.0f;
	//const float sigmaZ = 1.0f;
	const float sigmaN = 64.0f;
	const float sigmaZ = 3.0f;

	float weightN = pow(max(dot(centerN, sampleN), 0.0f), sigmaN);
	//For testing, disable normal weight
	//if(debug) weightN = 1.0f;
	float weightZ = exp(-abs(centerZ - sampleZ) / (centerZGradient * length(offset) * sigmaZ + EPSILON));
	//float weightL = 1.0f / (abs(centerL - sampleL) + EPSILON);
	float weight  = weightN * weightZ;
	return weight;
}

void SpatialResolve(sampler2D indirectDiffuse, sampler2D indirectSpecular, sampler2D depthTexture, sampler2D depthCoarseTexture, sampler2D normalTexture, sampler2D normalCoarseTexture, out vec4 upsampledIndirectDiffuse, out vec4 upsampledIndirectSpecular)
{
	vec4  centerValueIndirectDiffuse  = texture(indirectDiffuse,  texCoordsFrag);
	vec4  centerValueIndirectSpecular = texture(indirectSpecular, texCoordsFrag);
	vec3  centerNormal                = DecodeSignedOctahedronNormal(texture(normalTexture, texCoordsFrag).xy);
	float centerDepth                 = LinearizeDepth(texture(depthTexture, texCoordsFrag).x);
	float centerDepthGradient         = fwidth(centerDepth);

	upsampledIndirectDiffuse  = vec4(0.0f);
	upsampledIndirectSpecular = vec4(0.0f);
	float weight = 0.0f;
	for(int i = -SPATIAL_KERNEL_RADIUS; i <= SPATIAL_KERNEL_RADIUS; i++)
	{
		for(int j = -SPATIAL_KERNEL_RADIUS; j <= SPATIAL_KERNEL_RADIUS; j++)
		{
			ivec2 sampleOffset = ivec2(j, i);
			vec4  sampleValueIndirectDiffuse  = textureOffset(indirectDiffuse,  texCoordsFrag, sampleOffset);
			vec4  sampleValueIndirectSpecular = textureOffset(indirectSpecular, texCoordsFrag, sampleOffset);
			vec3  sampleNormal = DecodeSignedOctahedronNormal(textureLodOffset(normalCoarseTexture, texCoordsFrag, sampleTextureLod, sampleOffset).xy);
			float sampleDepth  = LinearizeDepth(textureLodOffset(depthCoarseTexture, texCoordsFrag, sampleTextureLod, sampleOffset).x);
			float sampleWeight = TapWeight(centerNormal, sampleNormal, centerDepth, sampleDepth, sampleOffset, centerDepthGradient);

			upsampledIndirectDiffuse  += sampleValueIndirectDiffuse  * sampleWeight;
			upsampledIndirectSpecular += sampleValueIndirectSpecular * sampleWeight;
			weight += sampleWeight;
		}
	}

	upsampledIndirectDiffuse  /= weight;
	upsampledIndirectSpecular /= weight;
	//This is not entirely necessary. Helps when there are no coherent samples found, but in practice this rarely occurs
	if(weight < EPSILON)
	{
		upsampledIndirectDiffuse  = centerValueIndirectDiffuse;
		upsampledIndirectSpecular = centerValueIndirectSpecular;
	}
}