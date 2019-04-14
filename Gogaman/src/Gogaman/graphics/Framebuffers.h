#pragma once

#include <glad.h>
#include "Gogaman/Config.h"

class Framebuffers
{
public:
	Framebuffers(Config &config);
public:
	unsigned int brdfFBO, brdfRBO;

	unsigned int gBuffer;
	unsigned int gPositionMetalness, gNormal, gAlbedoEmissivityRoughness, gVelocity, gDepth;

	unsigned int normalDownsampleFBO;
	unsigned int normalDownsampleBuffer;

	unsigned int depthDownsampleFBO;
	unsigned int depthDownsampleBuffer;

	unsigned int hdrFBO;
	unsigned int colorBuffers[3];
	unsigned int rboDepth;

	unsigned int previousFrameFBO;
	unsigned int previousFrameBuffer;

	unsigned int indirectFBO;
	unsigned int indirectLightingBuffers[2];

	unsigned int upsampleFBO;
	unsigned int upsampleBuffer;

	unsigned int upsampleFBO2;
	unsigned int upsampleBuffer2;

	unsigned int previousUpsampleFBO;
	unsigned int previousUpsampleBuffer;

	unsigned int previousUpsampleFBO2;
	unsigned int previousUpsampleBuffer2;

	unsigned int ssrFBO;
	unsigned int ssrBuffer;

	unsigned int cocFBO;
	unsigned int cocBuffer;

	unsigned int circularBlurHorizontalFBO;
	unsigned int circularBlurRedBuffer;
	unsigned int circularBlurGreenBuffer;
	unsigned int circularBlurBlueBuffer;

	unsigned int circularBlurVerticalFBO;
	unsigned int circularBlurVerticalBuffer;

	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];
private:
	void CheckCompleteness();
private:
	Config m_Config;

	const unsigned int oneColorAttachment       = GL_COLOR_ATTACHMENT0;
	const unsigned int twoColorAttachments[2]   = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	const unsigned int threeColorAttachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	const unsigned int fourColorAttachments[4]  = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
};