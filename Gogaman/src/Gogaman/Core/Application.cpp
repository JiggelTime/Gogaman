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
			
			ShaderID shv = g_Device->GetResources().shaders.Create(0, nullptr);
			ShaderID shp = g_Device->GetResources().shaders.Create(0, nullptr);

			ShaderProgramID shp0 = g_Device->GetResources().shaderPrograms.Create();
			g_Device->GetResources().shaderPrograms.Get(shp0).SetShader<Shader::Stage::Vertex>(shv);
			g_Device->GetResources().shaderPrograms.Get(shp0).SetShader<Shader::Stage::Pixel>(shp);

			TextureID tx0 = g_Device->GetResources().textures.Create(Texture::Format::XYZW8, m_Window->GetWidth(), m_Window->GetHeight());

			DescriptorGroupLayout::Binding bndg;
			bndg.descriptorCount = 1;
			bndg.index = 0;
			bndg.type = DescriptorHeap::Type::ShaderTexture;

			std::vector<DescriptorGroupLayout> dgls;
			dgls.emplace_back(1, &bndg, Shader::StageFlags::Pixel);

			RenderSurface::Attachment ca = { tx0, 1 };
			RenderSurfaceID rs0 = g_Device->GetResources().renderSurfaces.Create(1, &ca, RenderSurface::Attachment(), m_Window->GetWidth(), m_Window->GetHeight());

			RenderState::DepthStencilState dss;
			RenderState::BlendState        bs;
			RenderState state(dgls, shp0, rs0, dss, bs, m_Window->GetWidth(), m_Window->GetHeight());
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