#version 330 core
out vec4 FragColor;

in vec2 texCoordsFrag;

uniform sampler2D imageTexture;

uniform bool horizontal;
uniform float offset[3] = float[](0.0f, 1.3846153846f, 3.2307692308f);
uniform float weight[3] = float[](0.2270270270f, 0.3162162162f, 0.0702702703f);

void main()
{
	//Offset distance is the size of one texel
	vec2 texelSize = 1.0f / textureSize(imageTexture, 0);
	vec3 result = texture(imageTexture, texCoordsFrag).rgb * weight[0];
	if(horizontal)
	{
		for(int i = 1; i < 3; i++)
		{
			result += texture(imageTexture, texCoordsFrag + vec2(texelSize.x * offset[i], 0.0f)).rgb * weight[i];
			result += texture(imageTexture, texCoordsFrag - vec2(texelSize.x * offset[i], 0.0f)).rgb * weight[i];
		}
	}
	else
	{
		for(int i = 1; i < 3; i++)
		{
			result += texture(imageTexture, texCoordsFrag + vec2(0.0f, texelSize.y * offset[i])).rgb * weight[i];
			result += texture(imageTexture, texCoordsFrag - vec2(0.0f, texelSize.y * offset[i])).rgb * weight[i];
		}
	}

	FragColor = vec4(result, 1.0);
}