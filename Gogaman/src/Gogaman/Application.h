#pragma once

#include "Core.h"
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
		inline bool OnEvent(KeyPressEvent &event) { std::cout << "NIGGER" << std::endl; return true; }
	};

	Application *CreateApplication();
}