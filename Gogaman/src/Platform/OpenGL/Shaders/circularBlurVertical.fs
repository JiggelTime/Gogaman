#version 330 core

#define KERNEL_RADIUS 8

out vec4 FragColor;
//layout(location = 1) out vec4 BloomColor;

in vec2 texCoordsFrag;

uniform sampler2D cocTexture;
uniform sampler2D imageRedChannelTexture;
uniform sampler2D imageGreenChannelTexture;
uniform sampler2D imageBlueChannelTexture;

uniform bool debug;

vec2 multiplyComplex(vec2 p, vec2 q) { return vec2(p.x * q.x - p.y * q.y, p.x * q.y + p.y * q.x); }

//X real, Y imaginary
#define KERNEL0_WEIGHTS vec2(0.411259f, -0.548794f)
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

//X real, Y imaginary
#define KERNEL1_WEIGHTS vec2(0.513282f, 4.561110f)
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

	vec4 redChannelValue   = vec4(0.0f);
	vec4 greenChannelValue = vec4(0.0f);
	vec4 blueChannelValue  = vec4(0.0f);
	for(int i = -KERNEL_RADIUS; i <= KERNEL_RADIUS; i++)
	{
		vec2 sampleTexCoords = texCoordsFrag + vec2(0.0f, float(i)) * filterRadius * texelSize;
		sampleTexCoords      = clamp(sampleTexCoords, vec2(0.0f), vec2(1.0f));

		vec4 sampleRedChannelValue   = texture(imageRedChannelTexture,   sampleTexCoords);
		vec4 sampleGreenChannelValue = texture(imageGreenChannelTexture, sampleTexCoords);
		vec4 sampleBlueChannelValue  = texture(imageBlueChannelTexture,  sampleTexCoords);

		vec2 component0 = kernel0[i + KERNEL_RADIUS].xy;
		vec2 component1 = kernel1[i + KERNEL_RADIUS].xy;
		
		redChannelValue.xy   += multiplyComplex(sampleRedChannelValue.xy,   component0);
		redChannelValue.zw   += multiplyComplex(sampleRedChannelValue.zw,   component1);

		greenChannelValue.xy += multiplyComplex(sampleGreenChannelValue.xy, component0);
		greenChannelValue.zw += multiplyComplex(sampleGreenChannelValue.zw, component1);

		blueChannelValue.xy  += multiplyComplex(sampleBlueChannelValue.xy,  component0);
		blueChannelValue.zw  += multiplyComplex(sampleBlueChannelValue.zw,  component1);
	}

	vec3 result = vec3(0.0f);
	result.r = dot(redChannelValue.xy,   KERNEL0_WEIGHTS) + dot(redChannelValue.zw,   KERNEL1_WEIGHTS);
	result.g = dot(greenChannelValue.xy, KERNEL0_WEIGHTS) + dot(greenChannelValue.zw, KERNEL1_WEIGHTS);
	result.b = dot(blueChannelValue.xy,  KERNEL0_WEIGHTS) + dot(blueChannelValue.zw,  KERNEL1_WEIGHTS);

	if(debug) result = vec3(filterRadius);

	FragColor = vec4(result, 1.0f);
	//Write bright fragments to bloom buffer
	//float luminance = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	//BloomColor = luminance > BLOOM_THRESHOLD ? vec4(FragColor.rgb, 1.0f) : vec4(0.0f, 0.0f, 0.0f, 1.0f);
}