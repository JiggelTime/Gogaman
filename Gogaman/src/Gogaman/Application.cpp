#include "pch.h"
#include "Application.h"
#include "Graphics/Renderer.h"

namespace Gogaman
{
	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		Renderer gogaRenderer;
		gogaRenderer.Initialize();

		while(true)
		{
			gogaRenderer.Draw();
		}
	}
}