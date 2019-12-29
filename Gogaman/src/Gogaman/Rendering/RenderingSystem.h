#pragma once

#include "Gogaman/Core/Config.h"

#include "Gogaman/ECS/System.h"

#include "Gogaman/Events/EventListener.h"
#include "Gogaman/Events/WindowEvent.h"
#include "Gogaman/Events/KeyboardEvent.h"
#include "Gogaman/Events/MouseEvent.h"

#include "Gogaman/RenderHardwareInterface/DescriptorGroupLayout.h"
#include "Gogaman/RenderHardwareInterface/DescriptorHeap.h"
#include "Gogaman/RenderHardwareInterface/DescriptorGroup.h"
#include "Gogaman/RenderHardwareInterface/RenderState.h"
#include "Gogaman/RenderHardwareInterface/CommandBuffer.h"
#include "Gogaman/RenderHardwareInterface/CommandHeap.h"
#include "Gogaman/RenderHardwareInterface/Device.h"

#include "Gogaman/Geometry/Sphere.h"
#include "Gogaman/Geometry/RectangularFrustum.h"

#include "PerspectiveCamera.h"

#include "PBR_Material.h"

#include <glm.hpp>

namespace Gogaman
{
	class World;

	class RenderingSystem : public System, public EventListener
	{
	private:
		struct FrameData
		{
			glm::mat4 cameraViewMatrix;
			glm::mat4 cameraProjectionMatrix;
			glm::mat4 cameraViewProjectionMatrix;
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
		void InitializeRenderSurfaces();
		void InitializeShaders();

		void ImportFlexData();

		bool OnWindowResize(WindowResizeEvent &event);
	private:
		PerspectiveCamera m_Camera;

		//std::vector<RHI::RenderSurfaceID> m_RenderSurfaceIDs;

		RHI::ShaderProgramID m_ShaderID;

		std::vector<RHI::DescriptorGroupLayout> m_DescriptorGroupLayouts;

		std::vector<RHI::RenderState> m_RenderStates;

		std::unique_ptr<RHI::DescriptorHeap> m_DescriptorHeap;

		RHI::BufferID                         m_FrameDataBuffer;
		std::unique_ptr<RHI::DescriptorGroup> m_FrameDataDescriptorGroups[GM_SWAP_CHAIN_BUFFER_COUNT];
		
		std::vector<RHI::DescriptorGroup> m_MeshDataDescriptorGroups[GM_SWAP_CHAIN_BUFFER_COUNT];

		std::unique_ptr<RHI::CommandHeap>   m_TransferCommandHeap, m_RenderCommandHeap;
		std::unique_ptr<RHI::CommandBuffer> m_CommandBuffers[GM_SWAP_CHAIN_BUFFER_COUNT];

		std::vector<PBR_Material> m_Materials;

		uint16_t m_RenderResolutionWidth, m_RenderResolutionHeight;
	};
}