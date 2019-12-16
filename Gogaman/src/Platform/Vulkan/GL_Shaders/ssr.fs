#version 420 core

#define EPSILON 0.000001f

#define STEP_SIZE 1.0f
#define MAX_STEPS 2
#define NUM_BINARY_SEARCH_STEPS 2
#define RAY_HIT_THRESHOLD 0.9f
#define JITTER 0.01f
#define FADE_FRONT_FACING false

layout(location = 0) out vec4 FragColor;

in vec2 texCoordsFrag;

uniform mat3 transposeInverseV;
uniform mat4 P;
uniform mat4 inverseP;
uniform sampler2D renderedImageTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;

uniform bool debug;
uniform bool debug2;

bool  TexCoordsInvalid(vec2 texCoords) { return (any(greaterThan(texCoords, vec2(1.0f))) || any(lessThan(texCoords, vec2(0.0f)))); }
vec2  SignNotZero(vec2 v) { return vec2((v.x >= 0.0f) ? 1.0f : -1.0f, (v.y >= 0.0f) ? 1.0f : -1.0f); }
vec3  DecodeSignedOctahedronNormal(vec2 e);
float Rand(const vec2 coords) { return fract(sin(dot(coords.xy, vec2(12.9898f, 78.233f))) * 43758.5453f); }
vec3  FetchPosition(vec2 texCoords);
vec2  TexCoordsFromPosition(vec3 position);
vec4  RayBinarySearch(vec3 direction);
vec4  RayMarch(vec3 direction);

vec3 renderedImage = texture(renderedImageTexture, texCoordsFrag).rgb;

vec3 hitPosition;

void main()
{
	//View space position
	vec3 position = FetchPosition(texCoordsFrag);
	//View space normal
	vec3 normal   = transposeInverseV * DecodeSignedOctahedronNormal(texture(normalTexture, texCoordsFrag).xy);
	//Reflection vector
	vec3 reflectionDir = normalize(reflect(position, normal));

	hitPosition = position;
	vec3 rayDirection = reflectionDir * (1.0f - Rand(texCoordsFrag) * JITTER) * 2.0f;
	if(debug) rayDirection = reflectionDir * 2.0f;
	vec4 hitCoords = RayMarch(rayDirection);

	vec2 edgeFade = vec2(1.0f) - pow(clamp(abs(hitCoords.xy - vec2(0.5f)) * 2.0f, 0.0f, 1.0f), vec2(8.0f));
	float weightEdgeFade = clamp(min(edgeFade.x, edgeFade.y), 0.0f, 1.0f);
	float weightFrontFacing = FADE_FRONT_FACING ? clamp(-reflectionDir.z, 0.0f, 1.0f) : 1.0f;
	float weightBehindHeightfield = hitCoords.w;
	//Might not be necessary
	float weightTexCoordsValid = TexCoordsInvalid(hitCoords.xy) ? 0.0f : 1.0f;
	float weight = clamp(weightEdgeFade * weightFrontFacing * weightBehindHeightfield * weightTexCoordsValid, 0.0f, 1.0f);

	vec3 radiance = texture(renderedImageTexture, hitCoords.xy).rgb * weight;
	//radiance = vec3(fwidth(texture(depthTexture, texCoordsFrag).r));

	//vec3 resultValue = vec3(hitCoords.x, hitCoords.y, 0.0f);
	vec3 resultValue = radiance;
	if(!debug2) resultValue += renderedImage;

	resultValue = renderedImage;

	FragColor = vec4(resultValue, 1.0f);
}

vec3 DecodeSignedOctahedronNormal(vec2 e)
{
	vec3 v = vec3(e.xy, 1.0f - abs(e.x) - abs(e.y));
	if(v.z < 0.0f)
		v.xy = (1.0f - abs(v.yx)) * SignNotZero(v.xy);
	return normalize(v);
}

//Returns view space position
vec3 FetchPosition(vec2 texCoords)
{
	vec3 rawPos = vec3(texCoords, texture(depthTexture, texCoords).x);
	vec4 position = vec4(rawPos * 2.0f - 1.0f, 1.0f);
	position = inverseP * position;
	return position.xyz / position.w;
}

vec2 TexCoordsFromPosition(vec3 position)
{
	vec4 projectedPosition = P * vec4(position, 1.0f);
	//Perspective division
	projectedPosition.xy  /= projectedPosition.w;
	//[-1, 1] to [0, 1]
	projectedPosition.xy   = projectedPosition.xy * 0.5f + 0.5f;
	return projectedPosition.xy;
}

vec4 RayBinarySearch(vec3 direction)
{
	vec3 projectedPosition;
	float deltaDepth;
	for(int i = 0; i < NUM_BINARY_SEARCH_STEPS; i++)
	{
		projectedPosition = FetchPosition(TexCoordsFromPosition(hitPosition));
		deltaDepth = projectedPosition.z - hitPosition.z;

		if(deltaDepth < 0.0f)
			hitPosition += direction;
		direction *= 0.5f;
		hitPosition -= direction;
	}

	projectedPosition = FetchPosition(TexCoordsFromPosition(hitPosition));
	deltaDepth = projectedPosition.z - hitPosition.z;

	return vec4(TexCoordsFromPosition(hitPosition), hitPosition.z, abs(deltaDepth) < RAY_HIT_THRESHOLD ? 1.0f : 0.0f);
}

vec4 RayMarch(vec3 direction)
{
	vec3 projectedPosition;
	float deltaDepth;
	for(int i = 0; i < MAX_STEPS; i++)
	{
		hitPosition += direction;

		projectedPosition = FetchPosition(TexCoordsFromPosition(hitPosition));
		deltaDepth = projectedPosition.z - hitPosition.z;

		//Refine search if intersected
		if(deltaDepth > 0.0f)
			return RayBinarySearch(direction);

		direction *= STEP_SIZE;
	}
	
	return vec4(0.0f);
}