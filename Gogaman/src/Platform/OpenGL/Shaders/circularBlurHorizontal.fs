#version 330 core

layout(location = 0) out vec4 FragColorRed;
layout(location = 1) out vec4 FragColorGreen;
layout(location = 2) out vec4 FragColorBlue;

in vec2 texCoordsFrag;

uniform sampler2D cocTexture;
uniform sampler2D imageTexture;

uniform bool debug;

#define KERNEL_RADIUS 8

//XZ real, YW imaginary, XY non-bracketed, WZ bracketed
const vec4 kernel0[] = vec4[](
	vec4(0.014096f,  -0.022658f, 0.055991f, 0.004413f),
	vec4(-0.020612f, -0.025574f, 0.019188f, 0.000000f),
	vec4(-0.038708f,  0.006957f, 0.000000f, 0.049223f),
	vec4(-0.021449f,  0.040468f, 0.018301f, 0.099929f),
	vec4(0.013015f,   0.050223f, 0.054845f, 0.114689f),
	vec4(0.042178f,   0.038585f, 0.085769f, 0.097080f),
	vec4(0.057972f,   0.019812f, 0.102517f, 0.068674f),
	vec4(0.063647f,   0.005252f, 0.108535f, 0.046643f),
	vec4(0.064754f,   0.000000f, 0.109709f, 0.038697f),
	vec4(0.063647f,   0.005252f, 0.108535f, 0.046643f),
	vec4(0.057972f,   0.019812f, 0.102517f, 0.068674f),
	vec4(0.042178f,   0.038585f, 0.085769f, 0.097080f),
	vec4(0.013015f,   0.050223f, 0.054845f, 0.114689f),
	vec4(-0.021449f,  0.040468f, 0.018301f, 0.099929f),
	vec4(-0.038708f,  0.006957f, 0.000000f, 0.049223f),
	vec4(-0.020612f, -0.025574f, 0.019188f, 0.000000f),
	vec4(0.014096f,  -0.022658f, 0.055991f, 0.004413f)
);

//XZ real, YW imaginary, XY non-bracketed, WZ bracketed
const vec4 kernel1[] = vec4[](
	vec4(0.000115f, 0.009116f, 0.000000f, 0.051147f),
	vec4(0.005324f, 0.013416f, 0.009311f, 0.075276f),
	vec4(0.013753f, 0.016519f, 0.024376f, 0.092685f),
	vec4(0.024700f, 0.017215f, 0.043940f, 0.096591f),
	vec4(0.036693f, 0.015064f, 0.065375f, 0.084521f),
	vec4(0.047976f, 0.010684f, 0.085539f, 0.059948f),
	vec4(0.057015f, 0.005570f, 0.101695f, 0.031254f),
	vec4(0.062782f, 0.001529f, 0.112002f, 0.008578f),
	vec4(0.064754f, 0.000000f, 0.115526f, 0.000000f),
	vec4(0.062782f, 0.001529f, 0.112002f, 0.008578f),
	vec4(0.057015f, 0.005570f, 0.101695f, 0.031254f),
	vec4(0.047976f, 0.010684f, 0.085539f, 0.059948f),
	vec4(0.036693f, 0.015064f, 0.065375f, 0.084521f),
	vec4(0.024700f, 0.017215f, 0.043940f, 0.096591f),
	vec4(0.013753f, 0.016519f, 0.024376f, 0.092685f),
	vec4(0.005324f, 0.013416f, 0.009311f, 0.075276f),
	vec4(0.000115f, 0.009116f, 0.000000f, 0.051147f)
);

void main()
{
	vec2 texelSize = 1.0f / textureSize(cocTexture, 0);
	float filterRadius = texture(cocTexture, texCoordsFrag).r;

	vec4 resultValueRed   = vec4(0.0f);
	vec4 resultValueGreen = vec4(0.0f);
	vec4 resultValueBlue  = vec4(0.0f);
	for(int i = -KERNEL_RADIUS; i <= KERNEL_RADIUS; i++)
	{
		vec2 sampleTexCoords = texCoordsFrag + vec2(float(i), 0.0f) * filterRadius * texelSize;
		sampleTexCoords      = clamp(sampleTexCoords, vec2(0.0f), vec2(1.0f));
		vec3 sampleValue     = texture(imageTexture, sampleTexCoords).rgb;
		
		vec2 component0      = kernel0[i + KERNEL_RADIUS].xy;
		vec2 component1      = kernel1[i + KERNEL_RADIUS].xy;

		resultValueRed.xy   += sampleValue.r * component0;
		resultValueRed.zw   += sampleValue.r * component1;
		resultValueGreen.xy += sampleValue.g * component0;
		resultValueGreen.zw += sampleValue.g * component1;
		resultValueBlue.xy  += sampleValue.b * component0;
		resultValueBlue.zw  += sampleValue.b * component1;
	}

	FragColorRed   = resultValueRed;
	FragColorGreen = resultValueGreen;
	FragColorBlue  = resultValueBlue;
}