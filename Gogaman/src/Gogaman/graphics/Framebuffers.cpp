#include "pch.h"
#include "Framebuffers.h"
#include "Gogaman/Config.h"

namespace Gogaman
{
	uint Framebuffers::brdfFBO;
	uint Framebuffers::brdfRBO;

	uint Framebuffers::gBuffer;
	Texture2D Framebuffers::gPositionMetalness;
	Texture2D Framebuffers::gNormal;
	Texture2D Framebuffers::gAlbedoEmissivityRoughness;
	Texture2D Framebuffers::gVelocity;
	Texture2D Framebuffers::gDepth;

	uint Framebuffers::normalDownsampleFBO;
	Texture2D Framebuffers::normalDownsampleBuffer;

	uint Framebuffers::depthDownsampleFBO;
	Texture2D Framebuffers::depthDownsampleBuffer;

	uint Framebuffers::hdrFBO;
	Texture2D Framebuffers::colorBuffers[3];
	uint Framebuffers::rboDepth;

	uint Framebuffers::previousFrameFBO;
	Texture2D Framebuffers::previousFrameBuffer;

	uint Framebuffers::indirectFBO;
	Texture2D Framebuffers::indirectLightingBuffers[2];

	uint Framebuffers::upsampleFBO;
	Texture2D Framebuffers::upsampleBuffer;

	uint Framebuffers::upsampleFBO2;
	Texture2D Framebuffers::upsampleBuffer2;

	uint Framebuffers::previousUpsampleFBO;
	Texture2D Framebuffers::previousUpsampleBuffer;

	uint Framebuffers::previousUpsampleFBO2;
	Texture2D Framebuffers::previousUpsampleBuffer2;

	uint Framebuffers::ssrFBO;
	Texture2D Framebuffers::ssrBuffer;

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

		const uint renderResWidth  = GM_CONFIG.screenWidth  * GM_CONFIG.resScale;
		const uint renderResHeight = GM_CONFIG.screenHeight * GM_CONFIG.resScale;

		const uint giRenderResWidth  = GM_CONFIG.screenWidth  * GM_CONFIG.giResScale;
		const uint giRenderResHeight = GM_CONFIG.screenHeight * GM_CONFIG.giResScale;

		const uint dofRenderResWidth  = GM_CONFIG.screenWidth  * GM_CONFIG.dofResScale;
		const uint dofRenderResHeight = GM_CONFIG.screenHeight * GM_CONFIG.dofResScale;

		const uint bloomRenderResWidth  = GM_CONFIG.screenWidth  * GM_CONFIG.bloomResScale;
		const uint bloomRenderResHeight = GM_CONFIG.screenHeight * GM_CONFIG.bloomResScale;

		//Create precomputed BRDF framebuffer
		glGenFramebuffers(1, &brdfFBO);
		glGenRenderbuffers(1, &brdfRBO);
		glBindFramebuffer(GL_FRAMEBUFFER, brdfFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, brdfRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, brdfRBO);
		CheckCompleteness();

		//Create gBuffer
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		//Create position buffer
		gPositionMetalness.formatInternal = GL_RGBA16F;
		gPositionMetalness.formatImage = GL_RGBA;
		gPositionMetalness.filterMin = GL_NEAREST;
		gPositionMetalness.filterMag = GL_NEAREST;
		gPositionMetalness.Generate(renderResWidth, renderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionMetalness.GetID(), 0);
		//Create normal buffer
		gNormal.formatInternal = GL_RG16F;
		gNormal.formatImage = GL_RG;
		gNormal.filterMin = GL_NEAREST;
		gNormal.filterMag = GL_NEAREST;
		gNormal.Generate(renderResWidth, renderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal.GetID(), 0);
		//Create albedo and roughness buffer
		gAlbedoEmissivityRoughness.formatInternal = GL_RGBA8;
		gAlbedoEmissivityRoughness.formatImage = GL_RGBA;
		gAlbedoEmissivityRoughness.filterMin = GL_LINEAR;
		gAlbedoEmissivityRoughness.filterMag = GL_LINEAR;
		gAlbedoEmissivityRoughness.Generate(renderResWidth, renderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoEmissivityRoughness.GetID(), 0);
		//Create velocity buffer
		gVelocity.formatInternal = GL_RG16F;
		gVelocity.formatImage = GL_RG;
		gVelocity.filterMin = GL_NEAREST;
		gVelocity.filterMag = GL_NEAREST;
		gVelocity.Generate(renderResWidth, renderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gVelocity.GetID(), 0);
		//Create depth buffer
		gDepth.formatInternal = GL_DEPTH_COMPONENT32;
		gDepth.formatImage = GL_DEPTH_COMPONENT;
		gDepth.filterMin = GL_NEAREST;
		gDepth.filterMag = GL_NEAREST;
		gDepth.Generate(renderResWidth, renderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth.GetID(), 0);
		glDrawBuffers(4, fourColorAttachments);
		CheckCompleteness();

		//Create normal downsample FBO
		glGenFramebuffers(1, &normalDownsampleFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, normalDownsampleFBO);
		//Create quarter-resolution buffer
		normalDownsampleBuffer.formatInternal = GL_RG16F;
		normalDownsampleBuffer.formatImage = GL_RG;
		normalDownsampleBuffer.filterMin = GL_NEAREST_MIPMAP_NEAREST;
		normalDownsampleBuffer.filterMag = GL_NEAREST;
		normalDownsampleBuffer.levels = 0;
		normalDownsampleBuffer.Generate(renderResWidth * 0.5f, renderResHeight * 0.5f);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalDownsampleBuffer.GetID(), 0);
		glDrawBuffer(oneColorAttachment);
		CheckCompleteness();

		//Create depth downsample FBO
		glGenFramebuffers(1, &depthDownsampleFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, depthDownsampleFBO);
		//Create quarter-resolution buffer
		depthDownsampleBuffer.formatInternal = GL_R32F;
		depthDownsampleBuffer.formatImage = GL_RED;
		depthDownsampleBuffer.filterMin = GL_NEAREST_MIPMAP_NEAREST;
		depthDownsampleBuffer.filterMag = GL_NEAREST;
		depthDownsampleBuffer.levels = 0;
		depthDownsampleBuffer.Generate(renderResWidth* 0.5f, renderResHeight* 0.5f);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthDownsampleBuffer.GetID(), 0);
		glDrawBuffer(oneColorAttachment);
		CheckCompleteness();

		//Create HDR FBO
		glGenFramebuffers(1, &hdrFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		//Create 2 floating point buffers
		colorBuffers[0].formatInternal = GL_RGBA16F;
		colorBuffers[0].formatImage = GL_RGBA;
		colorBuffers[0].filterMin = GL_LINEAR;
		colorBuffers[0].filterMag = GL_LINEAR;
		colorBuffers[0].Generate(renderResWidth, renderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffers[0].GetID(), 0);
		colorBuffers[1].formatInternal = GL_RGBA16F;
		colorBuffers[1].formatImage = GL_RGBA;
		colorBuffers[1].filterMin = GL_LINEAR;
		colorBuffers[1].filterMag = GL_LINEAR;
		colorBuffers[1].Generate(renderResWidth, renderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, colorBuffers[1].GetID(), 0);
		glDrawBuffers(2, twoColorAttachments);
		//Create depth buffer
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		CheckCompleteness();

		//Create previous frame framebuffer
		glGenFramebuffers(1, &previousFrameFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, previousFrameFBO);
		//Create floating point buffer
		previousFrameBuffer.formatInternal = GL_RGBA16F;
		previousFrameBuffer.formatImage = GL_RGBA;
		previousFrameBuffer.filterMin = GL_NEAREST;
		previousFrameBuffer.filterMag = GL_NEAREST;
		previousFrameBuffer.Generate(renderResWidth, renderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, previousFrameBuffer.GetID(), 0);
		glDrawBuffer(oneColorAttachment);
		CheckCompleteness();

		//Create indirect lighting framebuffer
		glGenFramebuffers(1, &indirectFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, indirectFBO);
		//Create 2 floating point color buffers
		indirectLightingBuffers[0].formatInternal = GL_RGBA16F;
		indirectLightingBuffers[0].formatImage = GL_RGBA;
		indirectLightingBuffers[0].filterMin = GL_LINEAR;
		indirectLightingBuffers[0].filterMag = GL_LINEAR;
		indirectLightingBuffers[0].Generate(giRenderResWidth, giRenderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, indirectLightingBuffers[0].GetID(), 0);
		indirectLightingBuffers[1].formatInternal = GL_RGBA16F;
		indirectLightingBuffers[1].formatImage = GL_RGBA;
		indirectLightingBuffers[1].filterMin = GL_LINEAR;
		indirectLightingBuffers[1].filterMag = GL_LINEAR;
		indirectLightingBuffers[1].Generate(giRenderResWidth, giRenderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, indirectLightingBuffers[1].GetID(), 0);
		glDrawBuffers(2, twoColorAttachments);
		CheckCompleteness();

		//Create upsample framebuffer
		glGenFramebuffers(1, &upsampleFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, upsampleFBO);
		//Create floating point color buffer
		upsampleBuffer.formatInternal = GL_RGBA16F;
		upsampleBuffer.formatImage = GL_RGBA;
		upsampleBuffer.filterMin = GL_NEAREST;
		upsampleBuffer.filterMag = GL_NEAREST;
		upsampleBuffer.Generate(renderResWidth, renderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, upsampleBuffer.GetID(), 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create second upsample framebuffer
		glGenFramebuffers(1, &upsampleFBO2);
		glBindFramebuffer(GL_FRAMEBUFFER, upsampleFBO2);
		//Create floating point color buffer
		upsampleBuffer2.formatInternal = GL_RGBA16F;
		upsampleBuffer2.formatImage = GL_RGBA;
		upsampleBuffer2.filterMin = GL_NEAREST;
		upsampleBuffer2.filterMag = GL_NEAREST;
		upsampleBuffer2.Generate(renderResWidth, renderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, upsampleBuffer2.GetID(), 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create previous upsample framebuffer
		glGenFramebuffers(1, &previousUpsampleFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, previousUpsampleFBO);
		//Create floating point color buffer
		previousUpsampleBuffer.formatInternal = GL_RGBA16F;
		previousUpsampleBuffer.formatImage = GL_RGBA;
		previousUpsampleBuffer.filterMin = GL_LINEAR;
		previousUpsampleBuffer.filterMag = GL_LINEAR;
		previousUpsampleBuffer.Generate(renderResWidth, renderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, previousUpsampleBuffer.GetID(), 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create second previous upsample framebuffer
		glGenFramebuffers(1, &previousUpsampleFBO2);
		glBindFramebuffer(GL_FRAMEBUFFER, previousUpsampleFBO2);
		//Create floating point color buffer
		previousUpsampleBuffer2.formatInternal = GL_RGBA16F;
		previousUpsampleBuffer2.formatImage = GL_RGBA;
		previousUpsampleBuffer2.filterMin = GL_LINEAR;
		previousUpsampleBuffer2.filterMag = GL_LINEAR;
		previousUpsampleBuffer2.Generate(renderResWidth, renderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, previousUpsampleBuffer2.GetID(), 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create screen space reflections framebuffer
		glGenFramebuffers(1, &ssrFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, ssrFBO);
		//Create color buffer
		ssrBuffer.formatInternal = GL_RGBA16F;
		ssrBuffer.formatImage = GL_RGBA;
		ssrBuffer.filterMin = GL_NEAREST;
		ssrBuffer.filterMag = GL_NEAREST;
		ssrBuffer.Generate(renderResWidth, renderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssrBuffer.GetID(), 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create circle of confusion framebuffer
		glGenFramebuffers(1, &cocFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, cocFBO);
		//Create floating point buffer
		cocBuffer.formatInternal = GL_R16F;
		cocBuffer.formatImage = GL_RED;
		cocBuffer.filterMin = GL_NEAREST;
		cocBuffer.filterMag = GL_NEAREST;
		cocBuffer.Generate(dofRenderResWidth, dofRenderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cocBuffer.GetID(), 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create framebuffer for horizontal pass of separable circular blur
		glGenFramebuffers(1, &circularBlurHorizontalFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, circularBlurHorizontalFBO);
		//Create red channel floating point color buffer
		circularBlurRedBuffer.formatInternal = GL_RGBA16F;
		circularBlurRedBuffer.formatImage = GL_RGBA;
		circularBlurRedBuffer.filterMin = GL_NEAREST;
		circularBlurRedBuffer.filterMag = GL_NEAREST;
		circularBlurRedBuffer.Generate(dofRenderResWidth, dofRenderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, circularBlurRedBuffer.GetID(), 0);
		//Create green channel floating point color buffer
		circularBlurGreenBuffer.formatInternal = GL_RGBA16F;
		circularBlurGreenBuffer.formatImage = GL_RGBA;
		circularBlurGreenBuffer.filterMin = GL_NEAREST;
		circularBlurGreenBuffer.filterMag = GL_NEAREST;
		circularBlurGreenBuffer.Generate(dofRenderResWidth, dofRenderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, circularBlurGreenBuffer.GetID(), 0);
		//Create blue channel floating point color buffer
		circularBlurBlueBuffer.formatInternal = GL_RGBA16F;
		circularBlurBlueBuffer.formatImage = GL_RGBA;
		circularBlurBlueBuffer.filterMin = GL_NEAREST;
		circularBlurBlueBuffer.filterMag = GL_NEAREST;
		circularBlurBlueBuffer.Generate(dofRenderResWidth, dofRenderResHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, circularBlurBlueBuffer.GetID(), 0);
		glDrawBuffers(3, threeColorAttachments);
		CheckCompleteness();

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