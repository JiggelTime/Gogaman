#include "pch.h"
#include "Log.h"

namespace Gogaman
{
	std::shared_ptr<Logger> Log::s_CoreLogger(new Logger("GOGAMAN"));
	std::shared_ptr<Logger> Log::s_ClientLogger(new Logger("APPLICATION"));

	void Log::Initialize()
	{
		s_CoreLogger->SetLogLevel(LogLevel::Trace);
		s_ClientLogger->SetLogLevel(LogLevel::Trace);
	}
}