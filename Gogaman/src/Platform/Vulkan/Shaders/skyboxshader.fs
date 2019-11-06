#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 TexCoords;

uniform samplerCube skybox;

float Luminance(vec3 linearRGB) { return dot(linearRGB, vec3(0.3f, 0.59f, 0.11f)); }

void main()
{
    FragColor = texture(skybox, TexCoords);
	//Write bright fragments for bloom
	if(Luminance(FragColor.rgb) > 1.0f)
		BrightColor = vec4(FragColor.rgb, 1.0f);
	else
		BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}