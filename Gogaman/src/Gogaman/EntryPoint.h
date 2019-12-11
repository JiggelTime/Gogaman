#pragma once

#ifdef GM_PLATFORM_WINDOWS
	extern Gogaman::Application *Gogaman::CreateApplication();

	int main(int argc, char **argv)
	{
		auto application = Gogaman::CreateApplication();

		application->Run();

		delete application;
	}
#else
	#error
#endif