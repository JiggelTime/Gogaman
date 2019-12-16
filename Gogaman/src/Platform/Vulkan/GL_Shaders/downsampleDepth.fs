#version 420 core

out float DownsampledDepth;

in vec2 texCoordsFrag;

uniform int sampleMipLevel;
uniform sampler2D depthTexture;

uniform bool debug;

const ivec2 sampleKernel[] = ivec2[](ivec2(0, 0), ivec2(1, 0), ivec2(0, 1), ivec2(1, 1));

void main()
{
	float result = 1.0f;
	for(int i = 0; i < 4; i++)
	{
		float sampleDepth = textureLodOffset(depthTexture, texCoordsFrag, sampleMipLevel, sampleKernel[i]).x;
		if(sampleDepth < result)
			result = sampleDepth;
	}

	DownsampledDepth = result;
}