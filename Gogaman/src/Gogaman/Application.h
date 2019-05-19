#pragma once

#include "Core.h"
#include "Events/EventQueue.h"
#include "Events/KeyboardEvent.h"

namespace Gogaman
{
	class GOGAMAN_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	private:
	};

	Application *CreateApplication();
}