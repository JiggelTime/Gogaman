#pragma once

#include "Gogaman/Core/Config.h"

#include "Gogaman/ECS/System.h"

#include "Gogaman/Events/EventListener.h"
#include "Gogaman/Events/WindowEvent.h"
#include "Gogaman/Events/KeyboardEvent.h"
#include "Gogaman/Events/MouseEvent.h"

#include "Gogaman/RenderHardwareInterface/Identifier.h"
#include "Gogaman/RenderHardwareInterface/LinearFence.h"
#include "Gogaman/RenderHardwareInterface/DescriptorGroupLayout.h"
#include "Gogaman/RenderHardwareInterface/DescriptorHeap.h"
#include "Gogaman/RenderHardwareInterface/DescriptorGroup.h"
#include "Gogaman/RenderHardwareInterface/RenderState.h"
#include "Gogaman/RenderHardwareInterface/CommandBuffer.h"
#include "Gogaman/RenderHardwareInterface/CommandHeap.h"
#include "Gogaman/RenderHardwareInterface/Device.h"

#include "FrameContext.h"

#include "RenderGraph/ExecutableGraph.h"

#include "PerspectiveCamera.h"

#include "PBR_Material.h"

#include <glm.hpp>

namespace Gogaman
{
	class World;

	class RenderingSystem : public System, public EventListener
	{
	private:
		struct Mandelbrot
		{
			struct ShaderData
			{
				glm::vec3 positionAndZoom;
			};

			void Initialize();
			void CreateShaders();
			void CreateRenderState();
			void CreateRenderGraph();
			void Render(FrameContext &frameContext);

			RHI::ShaderID        v_MandelbrotShaderID, p_MandebrotShaderID;
			RHI::ShaderProgramID mandelbrotShaderProgramID;

			RHI::ShaderID        v_PostprocessShaderID, p_PostprocessShaderID;
			RHI::ShaderProgramID postprocessShaderProgramID;
			
			std::vector<RHI::RenderState> renderStates;

			std::unique_ptr<RHI::DescriptorHeap>    descriptorHeap;

			std::vector<RHI::DescriptorGroupLayout> mandelbulbDescriptorGroupLayouts;
			std::unique_ptr<RHI::DescriptorGroup>   mandelbulbDescriptorGroups[GM_RHI_CONCURRENT_FRAME_COUNT];

			std::vector<RHI::DescriptorGroupLayout> postprocessDescriptorGroupLayouts;
			std::unique_ptr<RHI::DescriptorGroup>   postprocessDescriptorGroups[GM_RHI_CONCURRENT_FRAME_COUNT];
			//std::unique_ptr<RHI::DescriptorGroup>   descriptorGroup;

			RHI::BufferID shaderDataBuffers[GM_RHI_CONCURRENT_FRAME_COUNT];

			std::unique_ptr<RHI::CommandHeap>   commandHeap;
			std::unique_ptr<RHI::CommandBuffer> commandBuffers[GM_RHI_CONCURRENT_FRAME_COUNT];

			RHI::SamplerID pointSampler;

			std::unique_ptr<RenderGraph::ExecutableGraph> renderGraph;

			glm::vec2 position;
			glm::vec2 smoothPosition;
			float     zoom;
			float     smoothZoom;
		};
	private:
		struct FrameData
		{
			glm::mat4 cameraViewMatrix;
			glm::mat4 cameraProjectionMatrix;
			glm::mat4 cameraViewProjectionMatrix;
			glm::vec3 cameraPosition;
		};

		struct MeshData
		{
			glm::mat4 meshMatrix;
		};
	public:
		RenderingSystem();

		virtual void Initialize() override;
		virtual void Update()     override;
		virtual void Render()     override;
		virtual void Shutdown()   override;

		virtual void OnEvent(Event &event) override;
	private:
		void CreateRenderSurfaces();

		void CreateShaders();
		void RecreateShaders();

		void CreateRenderState();

		void CreateRenderGraph();

		void ImportFlexData(const std::string &filepath);

		void Present();

		bool OnWindowFileDrop(WindowFileDropEvent &event);

		inline const FrameContext &GetCurrentFrameContext() const { return *m_FrameContexts[g_Device->GetNativeData().vulkanSwapChainImageIndex].get(); }
		inline       FrameContext &GetCurrentFrameContext()       { return *m_FrameContexts[g_Device->GetNativeData().vulkanSwapChainImageIndex].get(); }
	private:
		std::unique_ptr<RHI::LinearFence> m_FrameFence;
		std::unique_ptr<FrameContext>     m_FrameContexts[GM_RHI_CONCURRENT_FRAME_COUNT];

		PerspectiveCamera m_Camera;

		//std::vector<RHI::RenderSurfaceID> m_RenderSurfaceIDs;

		RHI::ShaderID        m_VertexShaderID, m_PixelShaderID;
		RHI::ShaderProgramID m_ShaderProgramID;

		std::vector<RHI::DescriptorGroupLayout> m_DescriptorGroupLayouts;

		std::vector<RHI::RenderState> m_RenderStates;

		std::unique_ptr<RHI::DescriptorHeap> m_DescriptorHeap;

		std::unique_ptr<RHI::DescriptorGroup> m_FrameDataDescriptorGroups[GM_RHI_CONCURRENT_FRAME_COUNT];
		RHI::BufferID                         m_FrameDataBuffers[GM_RHI_CONCURRENT_FRAME_COUNT];
		
		std::vector<RHI::DescriptorGroup> m_MaterialDataDescriptorGroups[GM_RHI_CONCURRENT_FRAME_COUNT];

		std::vector<RHI::DescriptorGroup> m_MeshDataDescriptorGroups[GM_RHI_CONCURRENT_FRAME_COUNT];

		//std::unique_ptr<RHI::CommandHeap>   m_TransferCommandHeap, m_RenderCommandHeap;
		//std::unique_ptr<RHI::CommandBuffer> m_CommandBuffers[GM_RHI_CONCURRENT_FRAME_COUNT];

		RHI::SamplerID m_PointSampler, m_AnisotropicSampler;

		std::vector<PBR_Material> m_Materials;

		RenderGraph::ExecutableGraph m_RenderGraph;

		Mandelbrot m_Mandelbrot;

		uint16_t m_RenderResolutionWidth, m_RenderResolutionHeight;
	};
}