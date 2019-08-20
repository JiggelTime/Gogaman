#include "pch.h"
#include "Log.h"

namespace Gogaman
{
	std::shared_ptr<Logger> Log::s_CoreLogger(std::make_shared<Logger>("GOGAMAN"));
	std::shared_ptr<Logger> Log::s_ClientLogger(std::make_shared<Logger>("APPLICATION"));

	void Log::Initialize()
	{
		s_CoreLogger->SetLogLevel(LogLevel::Trace);
		s_ClientLogger->SetLogLevel(LogLevel::Trace);
	}
}