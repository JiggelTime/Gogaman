#version 420 core

#define PI 3.14159265359f
#define EPSILON 0.000001f

#define USE_CLOSEST_VELOCITY 1
#define AABB_CLIP 1
//History blending weight when incoherent with current frame
#define WEIGHT_MINIMUM 0.87f
//History blending weight when coherent with current frame
#define WEIGHT_MAXIMUM 0.95f

layout(location = 0) out vec4 FragColor;

in vec2 texCoordsFrag;

uniform vec2 resolutionInput;
uniform vec2 resolutionOutput;
uniform vec2 texelSizeInput;
uniform vec2 texelSizeOutput;
uniform sampler2D inputTexture;
//Result from previous frame
uniform sampler2D previousInputTexture;
//Hyperbolic depth
uniform sampler2D depthTexture;
//Screen space velocity
uniform sampler2D velocityTexture;

uniform bool debug;
uniform bool debug2;

//https://gpuopen.com/optimized-reversible-tonemapper-for-resolve/
vec3  Tonemap(vec3 c) { return c / (max(c.r, max(c.g, c.b)) + 1.0f); }
vec3  InverseTonemap(vec3 c) { return c / (1.0f - max(c.r, max(c.g, c.b))); }
vec2  FetchClosestInverseVelocity(vec2 texCoords);
void  TightenAABBchroma(inout vec3 aabbAvg, inout vec3 aabbMin, inout vec3 aabbMax, vec3 currentPixelValue);
vec4  ClipAABB(vec3 neighborhoodMin, vec3 neighborhoodMax, vec4 currentValue, vec4 previousValue);
vec4  FetchSampleValue(sampler2D imageTexture, vec2 texCoords);
float UnbiasedLuminanceWeight(float currentLuminance, float previousLuminance, float maxLuminance);
vec4  TemporalResolve(sampler2D image, sampler2D imageHistory);

const mat3 RGBtoYCOCG = mat3(0.25f, 0.5f, -0.25f, 0.5f, 0.0f, 0.5f, 0.25f, -0.5f, -0.25f);
const mat3 YCOCGtoRGB = mat3(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 1.0f, -1.0f);
/*
vec2 resolutionInput  = textureSize(inputTexture,    0);
vec2 resolutionOutput = textureSize(velocityTexture, 0);
//Replace these with uniforms because division is slow
vec2 texelSizeInput  = 1.0f / resolutionInput;
vec2 texelSizeOutput = 1.0f / resolutionOutput;
*/
void main()
{
	vec4 result = TemporalResolve(inputTexture, previousInputTexture);
	result.a    = texture(inputTexture, texCoordsFrag).a;
	
	FragColor = result;
}

vec4 FetchSampleValue(sampler2D imageTexture, vec2 texCoords)
{
	vec4 value = texture(imageTexture, texCoords).rgba;
	value.xyz  = Tonemap(value.xyz);
	value.xyz  = RGBtoYCOCG * value.xyz;
	return value;
}

float UnbiasedLuminanceWeight(float currentLuminance, float previousLuminance, float maxLuminance)
{
	float unbiasedLumaDifference = abs(currentLuminance - previousLuminance) / max(currentLuminance, max(previousLuminance, maxLuminance));
	float weight                 = 1.0f - unbiasedLumaDifference;
	weight                       = mix(WEIGHT_MINIMUM, WEIGHT_MAXIMUM, weight * weight);
	return weight;
}

//https://github.com/playdeadgames/temporal/blob/master/Assets/Shaders/TemporalReprojection.shader
void TightenAABBchroma(inout vec3 aabbAvg, inout vec3 aabbMin, inout vec3 aabbMax, vec3 currentPixelValue)
{
	vec2 chromaExtent = vec2(0.25f * 0.5f * (aabbMax.x - aabbMin.x));
	vec2 chromaCenter = currentPixelValue.yz;

	aabbAvg.yz = chromaCenter;
	aabbMin.yz = chromaCenter - chromaExtent;
	aabbMax.yz = chromaCenter + chromaExtent;
}

//https://github.com/playdeadgames/temporal/blob/master/Assets/Shaders/TemporalReprojection.shader
vec3 ClipAABB(vec3 aabbMin, vec3 aabbMax, vec3 point)
{
	//Clips towards AABB center for better perfomance
	vec3  center   = 0.5f * (aabbMax + aabbMin);
	vec3  extents  = 0.5f * (aabbMax - aabbMin);
	//Relative position from center
	vec3  distance = point - center;
	vec3  ts       = abs(extents) / max(abs(distance), vec3(EPSILON));
	float t        = clamp(min(ts.x, min(ts.y, ts.z)), 0.0f, 1.0f);
	return center + distance * t;
}

//Obtains closest inverse velocity in 3x3 neighborhood
//NOTE: Using largest velocity instead of closest could be better
vec2 FetchClosestInverseVelocity(vec2 texCoords, vec2 texelSize)
{
	float closestDepth     = 1.0f + EPSILON;
	vec2  closestTexCoords = vec2(0.0f);

	//Search 3x3 neighborhood
	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			vec2  currentTexCoords = texCoords + (vec2(i, j) * texelSize);
			float currentDepth     = texture(depthTexture, currentTexCoords).x;
			if(currentDepth < closestDepth)
			{
				closestDepth     = currentDepth;
				closestTexCoords = currentTexCoords;
			}
		}
	}

	return texture(velocityTexture, closestTexCoords).xy;
}

vec4 TemporalResolve(sampler2D image, sampler2D imageHistory)
{
	//Screen space velocity
	#if USE_CLOSEST_VELOCITY
		vec2 velocity = FetchClosestInverseVelocity(texCoordsFrag, texelSizeOutput);
	#else
		vec2 velocity = texture(velocityTexture, texCoordsFrag).xy;
	#endif

	//Texture coordinate reprojection
	vec2 previousTexCoords  = texCoordsFrag - velocity;
	vec4 previousPixelValue = FetchSampleValue(imageHistory, previousTexCoords);

	vec2 texelSizeInputX = vec2(texelSizeInput.x, 0.0f);
	vec2 texelSizeInputY = vec2(0.0f, texelSizeInput.y);

	//Sample 3x3 neighborhood around current pixel
	vec4 nbrSamples[9];
	nbrSamples[0] = FetchSampleValue(image, texCoordsFrag - texelSizeInput);
	nbrSamples[1] = FetchSampleValue(image, texCoordsFrag - texelSizeInputY);
	nbrSamples[2] = FetchSampleValue(image, texCoordsFrag + texelSizeInputX - texelSizeInputY);
	nbrSamples[3] = FetchSampleValue(image, texCoordsFrag - texelSizeInputX);
	nbrSamples[4] = FetchSampleValue(image, texCoordsFrag);
	nbrSamples[5] = FetchSampleValue(image, texCoordsFrag + texelSizeInputX);
	nbrSamples[6] = FetchSampleValue(image, texCoordsFrag - texelSizeInputX + texelSizeInputY);
	nbrSamples[7] = FetchSampleValue(image, texCoordsFrag + texelSizeInputY);
	nbrSamples[8] = FetchSampleValue(image, texCoordsFrag + texelSizeInput);

	vec4 currentPixelValue = nbrSamples[4];

	vec4 neighborhoodAvg = nbrSamples[0];
	vec4 neighborhoodMin = nbrSamples[0];
	vec4 neighborhoodMax = nbrSamples[0];
	for(int i = 1; i < 9; i++)
	{
		neighborhoodAvg += nbrSamples[i];
		neighborhoodMin  = min(neighborhoodMin, nbrSamples[i]);
		neighborhoodMax  = max(neighborhoodMax, nbrSamples[i]);
	}

	neighborhoodAvg /= 9.0f;
	
	//Cross (+) shape neighborhood values
	vec4 neighborhoodCrossAvg = (nbrSamples[1] + nbrSamples[3] + nbrSamples[4] + nbrSamples[5] + nbrSamples[7]) / 5.0f;
	vec4 neighborhoodCrossMin = min(nbrSamples[1], min(nbrSamples[3], min(nbrSamples[4], min(nbrSamples[5], nbrSamples[7]))));
	vec4 neighborhoodCrossMax = max(nbrSamples[1], max(nbrSamples[3], max(nbrSamples[4], max(nbrSamples[5], nbrSamples[7]))));

	//Blend cross neighborhood values and 3x3 neighborhood values to round AABB shape
	neighborhoodAvg = (neighborhoodAvg + neighborhoodCrossAvg) * 0.5f;
	neighborhoodMin = (neighborhoodMin + neighborhoodCrossMin) * 0.5f;
	neighborhoodMax = (neighborhoodMax + neighborhoodCrossMax) * 0.5f;

	TightenAABBchroma(neighborhoodAvg.xyz, neighborhoodMin.xyz, neighborhoodMax.xyz, currentPixelValue.xyz);

	#if AABB_CLIP
		previousPixelValue.xyz = ClipAABB(neighborhoodMin.xyz, neighborhoodMax.xyz, previousPixelValue.xyz);
	#endif

	float blendWeight = UnbiasedLuminanceWeight(currentPixelValue.x, previousPixelValue.x, neighborhoodMax.x);
	blendWeight = WEIGHT_MAXIMUM;

	//http://advances.realtimerendering.com/s2014/epic/TemporalAA.pptx Slide 23
	//Guassian fit to Blackman-Harris 3.3 reconstruction filter (support is ~2 pixels wide)
	//Because box filter is not stable under motion
	/*
	float len = length(velocity);
	float W = exp(-2.29f * len * len);
	W *= 1.0f - clamp(len2 / maxLen, 0.0f, 1.0f);
	*/
	//TODO: Detect disocclusion: minimum depth in a 3x3 pixel neighborhood of the current frame is compared with the maximum depth in a corresponding 
	//2x2 pixel region in the previous frame. A disocclusion is detected when the current depth exceeds the previous depth

	//Fallback to current image if reprojected coordinates are offscreen
	if(any(greaterThan(previousTexCoords, vec2(1.0f)) || lessThan(previousTexCoords, vec2(0.0f))))
		blendWeight = 0.0f;

	vec4 resultValue = mix(currentPixelValue, previousPixelValue, blendWeight);
	resultValue.rgb  = YCOCGtoRGB * resultValue.xyz;
	resultValue.rgb  = InverseTonemap(clamp(resultValue.rgb, 0.0f, 1.0f));
	return resultValue;
}