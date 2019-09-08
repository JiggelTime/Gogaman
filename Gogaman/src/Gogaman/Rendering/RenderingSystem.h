#pragma once

#include "Gogaman/ECS/System.h"

#include "Gogaman/Events/EventListener.h"
#include "Gogaman/Events/WindowEvent.h"
#include "Gogaman/Events/KeyboardEvent.h"
#include "Gogaman/Events/MouseEvent.h"

#include "Gogaman/Core/Config.h"

#include "Gogaman/Geometry/Sphere.h"
#include "Gogaman/Geometry/RectangularFrustum.h"

#include "PerspectiveCamera.h"

#include "Shader/Shader.h"
#include "Shader/ShaderManager.h"

#include "Texture/Texture2D.h"
#include "Platform/OpenGL/OpenGL_Renderbuffer.h"
#include "RenderSurface.h"

#include "FullscreenTriangle.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glad.h>
#include <GLFW/glfw3.h>

namespace Gogaman
{
	class World;

	class RenderingSystem : public System, public EventListener
	{
	public:
		RenderingSystem();

		virtual void Initialize() override;
		virtual void Update()     override;
		virtual void Render()     override;
		virtual void Shutdown()   override;

		virtual void OnEvent(Event &event) override;
	private:
		void InitializeRenderSurfaces();
		void InitializeShaders();

		void RenderFullscreenWindow() const;

		void FrustumCull(std::vector<EntityID> &persistingEntities) const;

		bool OnWindowResize(WindowResizeEvent &event);
	private:
		PerspectiveCamera                   m_Camera;

		ShaderManager                       m_ShaderManager;
		ShaderID                            m_PrecomputeBRDFShader, m_GBufferShader, m_DeferredLightingShader, m_SkyboxShader, m_LightShader, m_PostprocessShader;

		std::unique_ptr<RenderSurface>      m_BRDF_Buffer;
		Texture2D                           m_BRDF_LUT;
		Renderbuffer                        m_BRDF_Depth;

		std::unique_ptr<RenderSurface>      m_G_Buffer;

		std::unique_ptr<RenderSurface>      m_FinalBuffer;

		std::unique_ptr<FullscreenTriangle> m_FullscreenTriangle;
	private:
		int m_RenderResolutionWidth;
		int m_RenderResolutionHeight;

		uint32_t frameCounter = 0;

		std::unordered_map<std::string, Texture2D>    m_Texture2Ds;
		std::unordered_map<std::string, Renderbuffer> m_Renderbuffers;
	};
}