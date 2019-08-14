#version 420 core

out vec2 DownsampledNormal;

in vec2 texCoordsFrag;

uniform int sampleMipLevel;
uniform sampler2D normalTexture;

uniform bool debug;

vec2 SignNotZero(vec2 v) { return vec2((v.x >= 0.0f) ? 1.0f : -1.0f, (v.y >= 0.0f) ? 1.0f : -1.0f); }
vec2 EncodeSignedOctahedronNormal(in vec3 v);
vec3 DecodeSignedOctahedronNormal(vec2 e);

const ivec2 sampleKernel[] = ivec2[](ivec2(0, 0), ivec2(1, 0), ivec2(0, 1), ivec2(1, 1));

void main()
{
	vec3 result = vec3(0.0f);
	for(int i = 0; i < 4; i++)
	{
		vec3 sampleNormal = DecodeSignedOctahedronNormal(textureLodOffset(normalTexture, texCoordsFrag, sampleMipLevel, sampleKernel[i]).xy);
		result += sampleNormal;
	}

	DownsampledNormal = EncodeSignedOctahedronNormal(normalize(result));
}

vec2 EncodeSignedOctahedronNormal(in vec3 v)
{
	//Project the sphere onto the octahedron, and then onto the xy plane 
	v.xy /= dot(abs(v), vec3(1.0f));
	//Reflect the folds of the lower hemisphere over the diagonals
	if(v.z <= 0.0f) v.xy = (1.0f - abs(v.yx)) * SignNotZero(v.xy);
	return v.xy;
}

vec3 DecodeSignedOctahedronNormal(vec2 e)
{
	vec3 v = vec3(e.xy, 1.0f - abs(e.x) - abs(e.y));
	if(v.z < 0.0f)
		v.xy = (1.0f - abs(v.yx)) * SignNotZero(v.xy);
	return normalize(v);
}