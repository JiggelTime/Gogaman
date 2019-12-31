#version 450

layout(location = 0) in vec2 p_UV;

layout(location = 0) out vec4 resultValue;

layout(set = 0, binding = 1) uniform sampler pointSampler;
layout(set = 0, binding = 2) uniform sampler anisotropicSampler;

layout(set = 1, binding = 0) uniform texture2D albedoTexture;
layout(set = 1, binding = 1) uniform texture2D normalTexture;
layout(set = 1, binding = 2) uniform texture2D roughnessTexture;
layout(set = 1, binding = 3) uniform texture2D metalnessTexture;
layout(set = 1, binding = 4) uniform texture2D emissivityTexture;

void main()
{
	resultValue = vec4(texture(sampler2D(albedoTexture, pointSampler), p_UV).rgb, 1.0f);
}