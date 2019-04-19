#include "pch.h"
#include "Framebuffers.h"

namespace Gogaman
{
	Framebuffers::Framebuffers(Config& config)
		: m_Config(config)
	{
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
		//Create position color buffer
		glGenTextures(1, &gPositionMetalness);
		glBindTexture(GL_TEXTURE_2D, gPositionMetalness);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionMetalness, 0);
		//Create normal buffer
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, GL_RG, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
		//Create albedo and roughness color buffer
		glGenTextures(1, &gAlbedoEmissivityRoughness);
		glBindTexture(GL_TEXTURE_2D, gAlbedoEmissivityRoughness);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoEmissivityRoughness, 0);
		//Create velocity buffer
		glGenTextures(1, &gVelocity);
		glBindTexture(GL_TEXTURE_2D, gVelocity);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, GL_RG, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gVelocity, 0);
		//Create depth buffer
		glGenTextures(1, &gDepth);
		glBindTexture(GL_TEXTURE_2D, gDepth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);
		glDrawBuffers(4, fourColorAttachments);
		CheckCompleteness();

		//Create normal downsample FBO
		glGenFramebuffers(1, &normalDownsampleFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, normalDownsampleFBO);
		//Create quarter-resolution color buffer
		glGenTextures(1, &normalDownsampleBuffer);
		glBindTexture(GL_TEXTURE_2D, normalDownsampleBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, config.screenWidth * config.resScale * 0.5f, config.screenHeight * config.resScale * 0.5f, 0, GL_RG, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalDownsampleBuffer, 0);
		glDrawBuffer(oneColorAttachment);
		CheckCompleteness();

		//Create depth downsample FBO
		glGenFramebuffers(1, &depthDownsampleFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, depthDownsampleFBO);
		//Create quarter-resolution color buffer
		glGenTextures(1, &depthDownsampleBuffer);
		glBindTexture(GL_TEXTURE_2D, depthDownsampleBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, config.screenWidth * config.resScale * 0.5f, config.screenHeight * config.resScale * 0.5f, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthDownsampleBuffer, 0);
		glDrawBuffer(oneColorAttachment);
		CheckCompleteness();

		//Create HDR FBO
		glGenFramebuffers(1, &hdrFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		//Create 2 floating point color buffers
		glGenTextures(2, colorBuffers);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
		}

		glDrawBuffers(2, twoColorAttachments);
		//Create depth buffer
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, config.screenWidth * config.resScale, config.screenHeight * config.resScale);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		CheckCompleteness();

		//Create previous frame framebuffer
		glGenFramebuffers(1, &previousFrameFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, previousFrameFBO);
		//Create floating point color buffers
		glGenTextures(1, &previousFrameBuffer);
		glBindTexture(GL_TEXTURE_2D, previousFrameBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, previousFrameBuffer, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create indirect lighting framebuffer
		glGenFramebuffers(1, &indirectFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, indirectFBO);
		//Create 2 floating point color buffers
		glGenTextures(2, indirectLightingBuffers);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, indirectLightingBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth * config.giResScale, config.screenHeight * config.giResScale, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, indirectLightingBuffers[i], 0);
		}

		glDrawBuffers(2, twoColorAttachments);
		CheckCompleteness();

		//Create upsample framebuffer
		glGenFramebuffers(1, &upsampleFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, upsampleFBO);
		//Create floating point color buffer
		glGenTextures(1, &upsampleBuffer);
		glBindTexture(GL_TEXTURE_2D, upsampleBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, upsampleBuffer, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create second upsample framebuffer
		glGenFramebuffers(1, &upsampleFBO2);
		glBindFramebuffer(GL_FRAMEBUFFER, upsampleFBO2);
		//Create floating point color buffer
		glGenTextures(1, &upsampleBuffer2);
		glBindTexture(GL_TEXTURE_2D, upsampleBuffer2);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, upsampleBuffer2, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create previous upsample framebuffer
		glGenFramebuffers(1, &previousUpsampleFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, previousUpsampleFBO);
		//Create floating point color buffer
		glGenTextures(1, &previousUpsampleBuffer);
		glBindTexture(GL_TEXTURE_2D, previousUpsampleBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, previousUpsampleBuffer, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create second previous upsample framebuffer
		glGenFramebuffers(1, &previousUpsampleFBO2);
		glBindFramebuffer(GL_FRAMEBUFFER, previousUpsampleFBO2);
		//Create floating point color buffer
		glGenTextures(1, &previousUpsampleBuffer2);
		glBindTexture(GL_TEXTURE_2D, previousUpsampleBuffer2);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, previousUpsampleBuffer2, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create screen space reflections framebuffer
		glGenFramebuffers(1, &ssrFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, ssrFBO);
		//Create color buffer
		glGenTextures(1, &ssrBuffer);
		glBindTexture(GL_TEXTURE_2D, ssrBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssrBuffer, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create circle of confusion framebuffer
		glGenFramebuffers(1, &cocFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, cocFBO);
		//Create floating point buffer
		glGenTextures(1, &cocBuffer);
		glBindTexture(GL_TEXTURE_2D, cocBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, config.screenWidth * config.dofResScale, config.screenHeight * config.dofResScale, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cocBuffer, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create framebuffer for horizontal pass of separable circular blur
		glGenFramebuffers(1, &circularBlurHorizontalFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, circularBlurHorizontalFBO);
		//Create red channel floating point color buffer
		glGenTextures(1, &circularBlurRedBuffer);
		glBindTexture(GL_TEXTURE_2D, circularBlurRedBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth * config.dofResScale, config.screenHeight * config.dofResScale, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, circularBlurRedBuffer, 0);
		//Create green channel floating point color buffer
		glGenTextures(1, &circularBlurGreenBuffer);
		glBindTexture(GL_TEXTURE_2D, circularBlurGreenBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth * config.dofResScale, config.screenHeight * config.dofResScale, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, circularBlurGreenBuffer, 0);
		//Create blue channel floating point color buffer
		glGenTextures(1, &circularBlurBlueBuffer);
		glBindTexture(GL_TEXTURE_2D, circularBlurBlueBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth * config.dofResScale, config.screenHeight * config.dofResScale, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, circularBlurBlueBuffer, 0);
		glDrawBuffers(3, threeColorAttachments);
		CheckCompleteness();

		//Create framebuffer for vertical pass of separable circular blur
		glGenFramebuffers(1, &circularBlurVerticalFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, circularBlurVerticalFBO);
		//Create floating point color buffer
		glGenTextures(1, &circularBlurVerticalBuffer);
		glBindTexture(GL_TEXTURE_2D, circularBlurVerticalBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth * config.dofResScale, config.screenHeight * config.dofResScale, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, circularBlurVerticalBuffer, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckCompleteness();

		//Create bloom guassian blur ping-pong framebuffers
		glGenFramebuffers(2, pingpongFBO);
		glGenTextures(2, pingpongColorbuffers);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, config.screenWidth * config.bloomResScale, config.screenHeight * config.bloomResScale, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
			CheckCompleteness();
		}
	}

	void Framebuffers::CheckCompleteness()
	{
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Error: Framebuffer incomplete" << std::endl;
	}
}