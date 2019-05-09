#include "pch.h"
#include "Framebuffers.h"
#include "Gogaman/Config.h"

namespace Gogaman
{
	//uint Framebuffers::upsampleFBO;
	//Texture2D Framebuffers::upsampleBuffer;

	//uint Framebuffers::upsampleFBO2;
	//Texture2D Framebuffers::upsampleBuffer2;

	//uint Framebuffers::previousUpsampleFBO;
	//Texture2D Framebuffers::previousUpsampleBuffer;

	//uint Framebuffers::previousUpsampleFBO2;
	//Texture2D Framebuffers::previousUpsampleBuffer2;

	//uint Framebuffers::ssrFBO;
	//Texture2D Framebuffers::ssrBuffer;

	uint Framebuffers::cocFBO;
	Texture2D Framebuffers::cocBuffer;

	uint Framebuffers::circularBlurHorizontalFBO;
	Texture2D Framebuffers::circularBlurRedBuffer;
	Texture2D Framebuffers::circularBlurGreenBuffer;
	Texture2D Framebuffers::circularBlurBlueBuffer;

	uint Framebuffers::circularBlurVerticalFBO;
	Texture2D Framebuffers::circularBlurVerticalBuffer;

	uint Framebuffers::pingpongFBO[2];
	Texture2D Framebuffers::pingpongColorbuffers[2];

	void Framebuffers::Initialize()
	{
		const uint oneColorAttachment       = GL_COLOR_ATTACHMENT0;
		const uint twoColorAttachments[2]   = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		const uint threeColorAttachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		const uint fourColorAttachments[4]  = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

		const uint renderResWidth       = GM_CONFIG.screenWidth  * GM_CONFIG.resScale;
		const uint renderResHeight      = GM_CONFIG.screenHeight * GM_CONFIG.resScale;
		
		const uint giRenderResWidth     = GM_CONFIG.screenWidth  * GM_CONFIG.giResScale;
		const uint giRenderResHeight    = GM_CONFIG.screenHeight * GM_CONFIG.giResScale;

		const uint dofRenderResWidth    = GM_CONFIG.screenWidth  * GM_CONFIG.dofResScale;
		const uint dofRenderResHeight   = GM_CONFIG.screenHeight * GM_CONFIG.dofResScale;

		const uint bloomRenderResWidth  = GM_CONFIG.screenWidth  * GM_CONFIG.bloomResScale;
		const uint bloomRenderResHeight = GM_CONFIG.screenHeight * GM_CONFIG.bloomResScale;

		//Create framebuffer for vertical pass of separable circular blur
		glGenFramebuffers(1, &circularBlurVerticalFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, circularBlurVerticalFBO);
		//Create floating point color buffer
		circularBlurVerticalBuffer.formatInternal = GL_RGBA16F;
		circularBlurVerticalBuffer.formatImage = GL_RGBA;
		circularBlurVerticalBuffer.filterMin = GL_LINEAR;
		circularBlurVerticalBuffer.filterMag = GL_LINEAR;
		circularBlurVerticalBuffer.Generate(dofRenderResWidth, dofRenderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, circularBlurVerticalBuffer.GetID(), 0);
		glDrawBuffer(oneColorAttachment);
		CheckCompleteness();

		//Create bloom guassian blur ping-pong framebuffers
		glGenFramebuffers(2, pingpongFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
		pingpongColorbuffers[0].formatInternal = GL_RGB16F;
		pingpongColorbuffers[0].formatImage = GL_RGB;
		pingpongColorbuffers[0].filterMin = GL_LINEAR;
		pingpongColorbuffers[0].filterMag = GL_LINEAR;
		pingpongColorbuffers[0].Generate(bloomRenderResWidth, bloomRenderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[0].GetID(), 0);
		CheckCompleteness();
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[1]);
		pingpongColorbuffers[1].formatInternal = GL_RGB16F;
		pingpongColorbuffers[1].formatImage = GL_RGB;
		pingpongColorbuffers[1].filterMin = GL_LINEAR;
		pingpongColorbuffers[1].filterMag = GL_LINEAR;
		pingpongColorbuffers[1].Generate(bloomRenderResWidth, bloomRenderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[1].GetID(), 0);
		CheckCompleteness();
	}

	void Framebuffers::CheckCompleteness()
	{
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Error: Framebuffer incomplete" << std::endl;
	}
}