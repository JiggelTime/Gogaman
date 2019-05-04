#pragma once

#include "Gogaman/Core.h"
#include "Framebuffer.h"
#include "Texture2D.h"
#include "Renderbuffer.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API Framebuffers
	{
	public:
		static void Initialize();
	public:
		static uint brdfFBO;
		static uint brdfRBO;
		//static Framebuffer brdfFBO;
		//static Renderbuffer brdfRBO;

		static uint gBuffer;
		static Texture2D gPositionMetalness;
		static Texture2D gNormal;
		static Texture2D gAlbedoEmissivityRoughness;
		static Texture2D gVelocity;
		static Texture2D gDepth;

		static uint normalDownsampleFBO;
		static Texture2D normalDownsampleBuffer;

		static uint depthDownsampleFBO;
		static Texture2D depthDownsampleBuffer;

		static uint hdrFBO;
		static Texture2D colorBuffers[3];
		static uint rboDepth;

		static uint previousFrameFBO;
		static Texture2D previousFrameBuffer;

		static uint indirectFBO;
		static Texture2D indirectLightingBuffers[2];

		static uint upsampleFBO;
		static Texture2D upsampleBuffer;

		static uint upsampleFBO2;
		static Texture2D upsampleBuffer2;

		static uint previousUpsampleFBO;
		static Texture2D previousUpsampleBuffer;

		static uint previousUpsampleFBO2;
		static Texture2D previousUpsampleBuffer2;

		static uint ssrFBO;
		static Texture2D ssrBuffer;

		static uint cocFBO;
		static Texture2D cocBuffer;

		static uint circularBlurHorizontalFBO;
		static Texture2D circularBlurRedBuffer;
		static Texture2D circularBlurGreenBuffer;
		static Texture2D circularBlurBlueBuffer;

		static uint circularBlurVerticalFBO;
		static Texture2D circularBlurVerticalBuffer;

		static uint pingpongFBO[2];
		static Texture2D pingpongColorbuffers[2];
	private:
		static void CheckCompleteness();
	};
}