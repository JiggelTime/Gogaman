#version 330 core

#define AUTO_FOCUS

out float FragColor;

in vec2 texCoordsFrag;

uniform float nearPlane;
uniform float farPlane;
uniform float focalDistance;
uniform float fStop;
uniform float focalLength;
uniform sampler2D depthTexture;

uniform bool debug;
uniform bool debug2;

float LinearizeDepth(float depth) { return -farPlane * nearPlane / (depth * (farPlane - nearPlane) - farPlane); }

void main()
{
	float linearDepth = LinearizeDepth(texture(depthTexture, texCoordsFrag).x);
	float objectDistance = linearDepth;
	#ifdef AUTO_FOCUS
		float focalDistance = LinearizeDepth(texture(depthTexture, vec2(0.5f, 0.5f)).x);
	#endif

	float cocRadius = (focalLength * focalLength * (focalDistance - objectDistance)) / (objectDistance * fStop * (focalDistance - focalLength));
	cocRadius = clamp(abs(cocRadius), 0.0f, 1.0f);

	//cocRadius = 0.5f;

	FragColor = cocRadius;
}