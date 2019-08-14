#version 330 core

layout(location = 0) out vec4 FragColor;

in vec2 texCoordsFrag;

uniform float bloomLuminanceCoeffecient;
uniform sampler2D imageTexture;
uniform sampler2D bloomTexture;

void main()
{
	vec3 image  = texture(imageTexture, texCoordsFrag).rgb;
	vec3 bloom  = texture(bloomTexture, texCoordsFrag).rgb;
	vec3 result = image + (bloom * bloomLuminanceCoeffecient);

	FragColor = vec4(result, 1.0f);
}