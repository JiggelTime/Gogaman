#version 420 core

layout(location = 0) out vec4 FragColor;

in vec2 texCoordsFrag;

uniform bool debug;
uniform bool debug2;

void main()
{
	vec3 radiance = texture(renderedImageTexture, texCoordsFrag).rgb;
	vec3 resultValue = radiance;

	FragColor = vec4(resultValue, 1.0f);
}