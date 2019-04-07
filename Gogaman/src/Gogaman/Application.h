#pragma once

#include "Core.h"

namespace Gogaman
{
	class GOGAMAN_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	Application *CreateApplication();
}