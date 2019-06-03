#pragma once

#include "Gogaman/Base.h"
#include "Logger.h"

namespace Gogaman
{
	class Log
	{
	public:
		static void Initialize();

		inline static std::shared_ptr<Logger> &GetCoreLogger()   { return s_CoreLogger; }
		inline static std::shared_ptr<Logger> &GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<Logger> s_CoreLogger;
		static std::shared_ptr<Logger> s_ClientLogger;
	};
}

//Macros
#define GM_LOG_CORE_SET_LEVEL(x) Gogaman::Log::GetCoreLogger()->SetLogLevel(Gogaman::LogLevel::x)
#define GM_LOG_CORE_GET_LEVEL    Gogaman::Log::GetCoreLogger()->GetLogLevel()

#define GM_LOG_CORE_TRACE(...)   Gogaman::Log::GetCoreLogger()->Log(Gogaman::LogLevel::Trace,   __VA_ARGS__)
#define GM_LOG_CORE_INFO(...)    Gogaman::Log::GetCoreLogger()->Log(Gogaman::LogLevel::Info,    __VA_ARGS__)
#define GM_LOG_CORE_WARNING(...) Gogaman::Log::GetCoreLogger()->Log(Gogaman::LogLevel::Warning, __VA_ARGS__)
#define GM_LOG_CORE_ERROR(...)   Gogaman::Log::GetCoreLogger()->Log(Gogaman::LogLevel::Error,   __VA_ARGS__)

#define GM_LOG_SET_LEVEL(x)      Gogaman::Log::GetClientLogger()->SetLogLevel(Gogaman::LogLevel::x)
#define GM_LOG_GET_LEVEL         Gogaman::Log::GetClientLogger()->GetLogLevel()

#define GM_LOG_TRACE(...)        Gogaman::Log::GetClientLogger()->Log(Gogaman::LogLevel::Trace,   __VA_ARGS__)
#define GM_LOG_INFO(...)         Gogaman::Log::GetClientLogger()->Log(Gogaman::LogLevel::Info,    __VA_ARGS__)
#define GM_LOG_WARNING(...)      Gogaman::Log::GetClientLogger()->Log(Gogaman::LogLevel::Warning, __VA_ARGS__)
#define GM_LOG_ERROR(...)        Gogaman::Log::GetClientLogger()->Log(Gogaman::LogLevel::Error,   __VA_ARGS__)