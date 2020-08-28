#version 450

layout(location = 0) in vec2 p_UV;

layout(location = 0) out vec4 resultValue;

layout(set = 0, binding = 0) uniform sampler pointSampler;
layout(set = 0, binding = 1) uniform texture2D inputTexture;

void main()
{
	float sampled = texture(sampler2D(inputTexture, pointSampler), p_UV).r;

	vec3 color = vec3(sampled, sampled / 10.0f, sampled * 5.0f);

	//Gamma correction
	color = pow(color, vec3(1.0f / 2.22f));

	resultValue = vec4(color, 1.0f);
}