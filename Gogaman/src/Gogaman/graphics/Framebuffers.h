#pragma once

#include "Gogaman/Core.h"
#include "Gogaman/Config.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API Framebuffers
	{
	public:
		static void Initialize(Config config);
	public:
		static unsigned int brdfFBO;
		static unsigned int brdfRBO;

		static unsigned int gBuffer;
		static unsigned int gPositionMetalness;
		static unsigned int gNormal;
		static unsigned int gAlbedoEmissivityRoughness;
		static unsigned int gVelocity;
		static unsigned int gDepth;

		static unsigned int normalDownsampleFBO;
		static unsigned int normalDownsampleBuffer;

		static unsigned int depthDownsampleFBO;
		static unsigned int depthDownsampleBuffer;

		static unsigned int hdrFBO;
		static unsigned int colorBuffers[3];
		static unsigned int rboDepth;

		static unsigned int previousFrameFBO;
		static unsigned int previousFrameBuffer;

		static unsigned int indirectFBO;
		static unsigned int indirectLightingBuffers[2];

		static unsigned int upsampleFBO;
		static unsigned int upsampleBuffer;

		static unsigned int upsampleFBO2;
		static unsigned int upsampleBuffer2;

		static unsigned int previousUpsampleFBO;
		static unsigned int previousUpsampleBuffer;

		static unsigned int previousUpsampleFBO2;
		static unsigned int previousUpsampleBuffer2;

		static unsigned int ssrFBO;
		static unsigned int ssrBuffer;

		static unsigned int cocFBO;
		static unsigned int cocBuffer;

		static unsigned int circularBlurHorizontalFBO;
		static unsigned int circularBlurRedBuffer;
		static unsigned int circularBlurGreenBuffer;
		static unsigned int circularBlurBlueBuffer;

		static unsigned int circularBlurVerticalFBO;
		static unsigned int circularBlurVerticalBuffer;

		static unsigned int pingpongFBO[2];
		static unsigned int pingpongColorbuffers[2];
	private:
		static void CheckCompleteness();
	};
}