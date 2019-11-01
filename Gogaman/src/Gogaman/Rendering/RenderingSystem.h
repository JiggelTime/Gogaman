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

#include "RenderingContext.h"

#include "Shader/Shader.h"
#include "Shader/ShaderManager.h"

#include "Texture/Texture2D.h"
#include "Platform/OpenGL/OpenGL_RenderBuffer.h"

#include "FullscreenTriangle.h"

#include "PBR_Material.h"

#include "RenderGraph/RenderGraph.h"

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
		void InitializeRenderGraph();

		void ImportFlexData();

		void GeometryPass();
		void DeferredShadingPass();
		void PostProcessPass();

		void RenderFullscreenWindow() const;

		void FrustumCull(std::vector<std::pair<EntityID, float>> &persistingEntities) const;

		bool OnWindowResize(WindowResizeEvent &event);
	private:
		RenderGraph::Graph                  m_RenderGraph;

		PerspectiveCamera                   m_Camera;

		ShaderManager                       m_ShaderManager;

		RenderSurfaceID                     m_PrecomputedBRDF;
		Texture2D                           m_BRDF_LUT;
		RenderBuffer                        m_BRDF_Depth;
		ShaderID                            m_PrecomputedBRDF_Shader;

		RenderSurfaceID                     m_G_Buffer;
		Texture2D_ID                        m_G_BufferPositionMetalness, m_G_BufferNormal, m_G_BufferAlbedoEmissivityRoughness, m_G_BufferVelocity, m_G_BufferDepth;
		ShaderID                            m_G_BufferShader;

		RenderSurfaceID                     m_FinalBuffer;
		Texture2D_ID                        m_FinalImage;
		RenderBuffer                        m_FinalImageDepth;

		ShaderID m_DeferredLightingShader, m_SkyboxShader, m_LightShader, m_PostprocessShader;

		std::unique_ptr<FullscreenTriangle> m_FullscreenTriangle;

		std::vector<PBR_Material>           m_Materials;
	private:
		int m_RenderResolutionWidth;
		int m_RenderResolutionHeight;

		uint32_t frameCounter = 0;
	};
}