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
#include "Gogaman/RenderHardwareInterface/Buffer.h"
#include "Gogaman/RenderHardwareInterface/Shader.h"
#include "Gogaman/RenderHardwareInterface/ShaderProgram.h"
#include "Gogaman/RenderHardwareInterface/VertexLayout.h"
#include "Gogaman/RenderHardwareInterface/RenderSurface.h"
#include "Gogaman/RenderHardwareInterface/DescriptorGroupLayout.h"
#include "Gogaman/RenderHardwareInterface/DescriptorHeap.h"
#include "Gogaman/RenderHardwareInterface/DescriptorGroup.h"
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

		VertexLayout vertexLayout({ { Shader::DataType::Float2 }, { Shader::DataType::Float3 } });
		
		float vertexData[3][5];
		//V0
		vertexData[0][0] =  0.0f;
		vertexData[0][1] = -0.5f;

		vertexData[0][2] = 1.0f;
		vertexData[0][3] = 0.0f;
		vertexData[0][4] = 0.0f;
		//V1
		vertexData[1][0] =  0.5f;
		vertexData[1][1] =  0.5f;

		vertexData[1][2] = 0.0f;
		vertexData[1][3] = 1.0f;
		vertexData[1][4] = 0.0f;
		//V2
		vertexData[2][0] = -0.5f;
		vertexData[2][1] =  0.5f;

		vertexData[2][2] = 0.0f;
		vertexData[2][3] = 0.0f;
		vertexData[2][4] = 1.0f;

		BufferID vertexBufferID = g_Device->GetResources().buffers.Create(sizeof(vertexData), Buffer::BindFlags::Vertex);
		auto &vertexBuffer = g_Device->GetResources().buffers.Get(vertexBufferID);
		vertexBuffer.SetData(&vertexData, vertexBuffer.GetSize());

		DescriptorGroupLayout::Binding *bndg = new DescriptorGroupLayout::Binding;
		bndg->descriptorCount = 1;
		bndg->type            = DescriptorHeap::Type::ShaderConstantBuffer;

		std::vector<DescriptorGroupLayout> dgls;
		dgls.emplace_back(1, bndg, Shader::StageFlags::Pixel);

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

		std::vector<RenderState> states;
		states.reserve(GM_SWAP_CHAIN_BUFFER_COUNT);
		for(uint8_t i = 0; i < GM_SWAP_CHAIN_BUFFER_COUNT; i++)
		{
			states.emplace_back(dgls, vertexLayout, shp0, rss[i], dss, bs, m_Window->GetWidth(), m_Window->GetHeight());
		}

		DescriptorHeap::DescriptorCounts dschc;
		dschc.SetDescriptorCount<DescriptorHeap::Type::ShaderConstantBuffer>(GM_SWAP_CHAIN_BUFFER_COUNT);
		DescriptorHeap dsch(std::move(dschc));

		struct PerFrameData
		{
			float color[3];
		};

		PerFrameData data;

		BufferID shaderConstantBufferID = g_Device->GetResources().buffers.Create(sizeof(PerFrameData), Buffer::BindFlags::ShaderConstants);
		Buffer &shaderConstantBuffer = g_Device->GetResources().buffers.Get(shaderConstantBufferID);

		std::unique_ptr<DescriptorGroup> descriptorGroups[GM_SWAP_CHAIN_BUFFER_COUNT];

		CommandHeap cmdh(CommandHeap::Type::Direct);

		std::unique_ptr<CommandBuffer> cmdbs[GM_SWAP_CHAIN_BUFFER_COUNT];
		for(uint8_t i = 0; i < GM_SWAP_CHAIN_BUFFER_COUNT; i++)
		{
			descriptorGroups[i] = std::make_unique<DescriptorGroup>(&dsch, &dgls[0]);

			cmdbs[i] = cmdh.CreateCommandBuffer();
		}

		while(m_IsRunning)
		{
			Time::Update();
			Input::Update();

			m_World.Update();
			m_World.Render();

			const auto swapChainImageIndex = g_Device->GetNativeData().vulkanSwapChainImageIndex;

			data.color[0] = cos(Input::GetMousePosition().x / 100.0f);
			data.color[1] = sin(Input::GetMousePosition().y / 100.0f);
			data.color[2] = sin(Time::GetTime() * 3.0f);
			shaderConstantBuffer.SetData(&data, shaderConstantBuffer.GetSize());
			descriptorGroups[swapChainImageIndex]->SetShaderConstantBuffer(0, shaderConstantBufferID);

			RenderCommandRecorder cmdr(cmdbs[swapChainImageIndex].get(), &states[swapChainImageIndex]);
			cmdr.BindDescriptorGroup(0, *descriptorGroups[swapChainImageIndex].get());

			cmdr.BindBuffer(0, vertexBuffer);

			cmdr.Render(3, 0);
			cmdr.StopRecording();

			g_Device->Submit(CommandHeap::Type::Direct, 1, cmdbs[swapChainImageIndex].get());
			
			m_Window->Update();

			EventManager::GetInstance().DispatchEvents();
		}
	}
}