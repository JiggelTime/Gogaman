#pragma once

#include "Logger.h"

namespace Gogaman
{
	namespace Log
	{
		static std::shared_ptr<Logger> s_CoreLogger(std::make_shared<Logger>("Gogaman"));
		static std::shared_ptr<Logger> s_ClientLogger(std::make_shared<Logger>("Application"));
	}
	//class Log
	//{
	//public:
	//	static inline constexpr std::shared_ptr<Logger> &GetCoreLogger()   { return s_CoreLogger;   }
	//	static inline constexpr std::shared_ptr<Logger> &GetClientLogger() { return s_ClientLogger; }
	//private:
	//	static std::shared_ptr<Logger> s_CoreLogger;
	//	static std::shared_ptr<Logger> s_ClientLogger;
	//};

}

//Core logger
#define GM_LOG_CORE_SET_LEVEL(x) Gogaman::Log::s_CoreLogger->SetLogLevel(Gogaman::LogLevel::x)
#define GM_LOG_CORE_GET_LEVEL    Gogaman::Log::s_CoreLogger->GetLogLevel()

#define GM_LOG_CORE_TRACE(...)   Gogaman::Log::s_CoreLogger->Log(Gogaman::LogLevel::Trace,   __VA_ARGS__);
#define GM_LOG_CORE_INFO(...)    Gogaman::Log::s_CoreLogger->Log(Gogaman::LogLevel::Info,    __VA_ARGS__);
#define GM_LOG_CORE_WARNING(...) Gogaman::Log::s_CoreLogger->Log(Gogaman::LogLevel::Warning, __VA_ARGS__);
#define GM_LOG_CORE_ERROR(...)   Gogaman::Log::s_CoreLogger->Log(Gogaman::LogLevel::Error,   __VA_ARGS__);

//Client logger
#define GM_LOG_SET_LEVEL(x)      Gogaman::Log::s_CoreLogger->SetLogLevel(Gogaman::LogLevel::x)
#define GM_LOG_GET_LEVEL         Gogaman::Log::s_CoreLogger->GetLogLevel()

#define GM_LOG_TRACE(...)        Gogaman::Log::s_CoreLogger->Log(Gogaman::LogLevel::Trace,   __VA_ARGS__);
#define GM_LOG_INFO(...)         Gogaman::Log::s_CoreLogger->Log(Gogaman::LogLevel::Info,    __VA_ARGS__);
#define GM_LOG_WARNING(...)      Gogaman::Log::s_CoreLogger->Log(Gogaman::LogLevel::Warning, __VA_ARGS__);
#define GM_LOG_ERROR(...)        Gogaman::Log::s_CoreLogger->Log(Gogaman::LogLevel::Error,   __VA_ARGS__);