#pragma once

#include "Gogaman/Rendering/AbstractRenderingContext.h"

#include <glad.h>
#include <GLFW/glfw3.h>

namespace Gogaman
{
	class Window;

	class RenderingContext : public AbstractRenderingContext<RenderingContext>
	{
	public:
		RenderingContext(Window *window);
		~RenderingContext();

		void Initialize();

		inline void SwapBuffers() { glfwSwapBuffers(m_GLFW_Window); }

		inline void RenderIndexed(const uint32_t indexCount) const { glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0); }
		
		inline void DispatchComputeShader(const uint16_t threadGroupCountX, const uint16_t threadGroupCountY, const uint16_t threadGroupCountZ) const { glDispatchCompute(threadGroupCountX, threadGroupCountY, threadGroupCountZ); }
	
		inline void EnableDepthTesting()  const { glEnable(GL_DEPTH_TEST);  }
		inline void DisableDepthTesting() const { glDisable(GL_DEPTH_TEST); }

		inline void SetDepthComparisonMode(const RenderState::DepthComparisonMode comparisonMode) const { glDepthFunc(RenderState::GetNativeDepthComparisonMode(comparisonMode)); }

		inline void EnableBlending()  const { glEnable(GL_BLEND);  }
		inline void DisableBlending() const { glDisable(GL_BLEND); }

		inline void SetBlendState(const RenderState::BlendFactor sourceBlendFactor, const RenderState::BlendFactor destinationBlendFactor) const { glBlendFunc(RenderState::GetNativeBlendFactor(sourceBlendFactor), RenderState::GetNativeBlendFactor(destinationBlendFactor)); }

		inline void SetFaceWindingOrder(const RenderState::FaceWindingOrder windingOrder) const { glFrontFace(RenderState::GetNativeFaceWindingOrder(windingOrder)); }
	private:
		GLFWwindow *m_GLFW_Window;
	};
}