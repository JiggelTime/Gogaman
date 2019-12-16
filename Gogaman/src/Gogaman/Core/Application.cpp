#include "pch.h"
#include "Application.h"

#include "Logging/Log.h"
#include "Base.h"

#include "Config.h"
#include "Time.h"

#include "Gogaman/Input/Input.h"

#include "Gogaman/Events/EventManager.h"

//#include "Rendering/ModelMatrixSystem.h"
#include "Gogaman/BoundingVolumeSystem.h"
//#include "Rendering/RenderingSystem.h"

//TEST INCLUDES
#include "Gogaman/RenderHardwareInterface/Identifier.h"
#include "Gogaman/RenderHardwareInterface/Texture.h"
#include "Gogaman/RenderHardwareInterface/Shader.h"
#include "Gogaman/RenderHardwareInterface/ShaderProgram.h"
#include "Gogaman/RenderHardwareInterface/RenderSurface.h"
#include "Gogaman/RenderHardwareInterface/DescriptorGroupLayout.h"
#include "Gogaman/RenderHardwareInterface/RenderState.h"
#include "Gogaman/RenderHardwareInterface/CommandBuffer.h"
#include "Gogaman/RenderHardwareInterface/CommandHeap.h"
#include "Gogaman/RenderHardwareInterface/RenderCommandRecorder.h"
#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	Application *Application::s_Instance = nullptr;

	Application::Application()
		: m_IsRunning(true)
	{
		GM_ASSERT(s_Instance == nullptr, "Failed to construct application | Instance already exists");
		s_Instance = this;

		GM_LOG_CORE_SET_LEVEL(Info);

		m_Window = std::make_unique<Window>("Gogaman", GM_CONFIG.screenWidth, GM_CONFIG.screenHeight, VerticalSynchronization::Disabled);

		//m_World.AddSystem(std::make_unique<ModelMatrixSystem>());
		m_World.AddSystem(std::make_unique<BoundingVolumeSystem>());
		//m_World.AddSystem(std::make_unique<RenderingSystem>());
		m_World.Initialize();
	}

	Application::~Application()
	{
		m_World.Shutdown();

		m_Window.reset();
		Window::Shutdown();
	}
	
	void Application::Run()
	{
		{
			using namespace RHI;
			
			auto LoadShader = [](const char *filepath)
			{
				FILE *file = fopen(filepath, "rb");
				fseek(file, 0, SEEK_END);
				uint32_t size = ftell(file);
				fseek(file, 0, SEEK_SET);
				uint8_t *buffer = new uint8_t[size];
				fread(buffer, size, 1, file);
				fclose(file);

				return std::pair<uint32_t, uint8_t *>(size, buffer);
			};

			auto shvdata = LoadShader("D:/dev/Gogaman/Gogaman/Shaders/vert.spv");
			ShaderID shv = g_Device->GetResources().shaders.Create(shvdata.first, shvdata.second);

			auto shpdata = LoadShader("D:/dev/Gogaman/Gogaman/Shaders/frag.spv");
			ShaderID shp = g_Device->GetResources().shaders.Create(shpdata.first, shpdata.second);

			ShaderProgramID shp0 = g_Device->GetResources().shaderPrograms.Create();
			g_Device->GetResources().shaderPrograms.Get(shp0).SetShader<Shader::Stage::Vertex>(shv);
			g_Device->GetResources().shaderPrograms.Get(shp0).SetShader<Shader::Stage::Pixel>(shp);

			//DescriptorGroupLayout::Binding *bndg = new DescriptorGroupLayout::Binding;
			//bndg->descriptorCount = 1;
			//bndg->index = 0;
			//bndg->type = DescriptorHeap::Type::ShaderTexture;

			std::vector<DescriptorGroupLayout> dgls;
			//dgls.emplace_back(1, bndg, Shader::StageFlags::Pixel);

			std::vector<RenderSurfaceID> rss;
			for(const auto &i : g_Device->GetNativeData().vulkanSwapChainImageViews)
			{
				TextureID tx0 = g_Device->GetResources().textures.Create(Texture::Format::XYZW8, m_Window->GetWidth(), m_Window->GetHeight());
				g_Device->GetResources().textures.Get(tx0).GetNativeData().vulkanImageView = i;
				RenderSurface::Attachment ca = { tx0, 1 };

				rss.emplace_back(g_Device->GetResources().renderSurfaces.Create(1, &ca, RenderSurface::Attachment(), m_Window->GetWidth(), m_Window->GetHeight()));
			}

			RenderState::DepthStencilState dss;
			RenderState::BlendState        bs;
			RenderState state(dgls, shp0, rss[0], dss, bs, m_Window->GetWidth(), m_Window->GetHeight());

			CommandHeap   cmdh(CommandHeap::Type::Direct);
			std::unique_ptr<CommandBuffer> cmdb = cmdh.CreateCommandBuffer();

			RenderCommandRecorder cmdr(cmdb.get(), &state);
			cmdr.Render(3, 0);
			cmdr.StopRecording();

			const auto &vulkanDevice = g_Device->GetNativeData().vulkanDevice;

			VkSemaphoreCreateInfo semaphoreDescriptor = {};
			semaphoreDescriptor.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			VkSemaphore startSemaphore, stopSemaphore;
			vkCreateSemaphore(vulkanDevice, &semaphoreDescriptor, nullptr, &startSemaphore);
			vkCreateSemaphore(vulkanDevice, &semaphoreDescriptor, nullptr, &stopSemaphore);

			uint32_t vulkanImageIndex;
			vkAcquireNextImageKHR(vulkanDevice, g_Device->GetNativeData().vulkanSwapChain, UINT64_MAX, startSemaphore, VK_NULL_HANDLE, &vulkanImageIndex);

			VkSubmitInfo submitDescriptor = {};
			submitDescriptor.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitDescriptor.waitSemaphoreCount     = 1;
			submitDescriptor.pWaitSemaphores        = &startSemaphore;
			VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			submitDescriptor.pWaitDstStageMask      = &pipelineStageFlags;
			submitDescriptor.commandBufferCount     = 1;
			submitDescriptor.pCommandBuffers        = &cmdb->GetNativeData().vulkanCommandBuffer;
			submitDescriptor.signalSemaphoreCount   = 1;
			submitDescriptor.pSignalSemaphores      = &stopSemaphore;

			//const VkQueue &queue = g_Device->GetNativeData().vulkanQueues[g_Device->GetNativeCommandHeapType(CommandHeap::Type::Direct)];
			const VkQueue &queue = g_Device->GetNativeData().vulkanQueues[2];

			vkQueueSubmit(queue, 1, &submitDescriptor, VK_NULL_HANDLE);

			VkPresentInfoKHR presentDescriptor = {};
			presentDescriptor.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentDescriptor.waitSemaphoreCount = 1;
			presentDescriptor.pWaitSemaphores    = &stopSemaphore;
			presentDescriptor.swapchainCount     = 1;
			presentDescriptor.pSwapchains        = &g_Device->GetNativeData().vulkanSwapChain;
			presentDescriptor.pImageIndices      = &vulkanImageIndex;

			vkQueuePresentKHR(queue, &presentDescriptor);

			vkDestroySemaphore(vulkanDevice, startSemaphore, nullptr);
			vkDestroySemaphore(vulkanDevice, stopSemaphore, nullptr);
		}

		while(m_IsRunning)
		{
			Time::Update();
			Input::Update();

			m_World.Update();
			m_World.Render();
			
			//m_Window->Update();

			EventManager::GetInstance().DispatchEvents();
		}
	}
}