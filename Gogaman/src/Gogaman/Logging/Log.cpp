#include "pch.h"
#include "Log.h"

#include "ConsoleLogger.h"
#include "FileLogger.h"

namespace Gogaman
{
	std::shared_ptr<AbstractLogger> Log::s_CoreLogger(new   ConsoleLogger("GOGAMAN"));
	std::shared_ptr<AbstractLogger> Log::s_ClientLogger(new ConsoleLogger("APPLICATION"));
	std::shared_ptr<AbstractLogger> Log::s_FileLogger(new   FileLogger("TEST"));

	void Log::Initialize()
	{
		s_CoreLogger->SetLogLevel(LogLevel::Trace);
		s_ClientLogger->SetLogLevel(LogLevel::Trace);
		s_FileLogger->SetLogLevel(LogLevel::Trace);
	}
}