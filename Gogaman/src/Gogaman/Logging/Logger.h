#pragma once

#include "pch.h"
#include "Gogaman/Core.h"

namespace Gogaman
{
	enum class LogLevel
	{
		LogInfo, LogWarning, LogError
	};

	class GOGAMAN_API Logger
	{
	public:
		void Log(const LogLevel level, const char *format, ...);
		void Log(const LogLevel level, const std::string &message);

		static Logger *GetLogger();

		void SetLogLevel(LogLevel level);
		LogLevel GetLogLevel();
	private:
		Logger();
		~Logger();
	private:
		static Logger *s_Instance;
		static LogLevel m_LogLevel;
	};
}

//Log macros
#define GM_LOG_SET_LEVEL(x)    Gogaman::Logger::GetLogger()->SetLogLevel(x)
#define GM_LOG_GET_LEVEL       Gogaman::Logger::GetLogger()->GetLogLevel()

#define GM_LOG_INFO(x)         Gogaman::Logger::GetLogger()->Log(LogLevel::LogInfo, x)
#define GM_LOG_WARNING(x)      Gogaman::Logger::GetLogger()->Log(LogLevel::LogWarning, x)
#define GM_LOG_ERROR(x)        Gogaman::Logger::GetLogger()->Log(LogLevel::LogError, x)

#define GM_LOG_INFO(x, ...)    Gogaman::Logger::GetLogger()->Log(LogLevel::LogInfo, x, __VA_ARGS__)
#define GM_LOG_WARNING(x, ...) Gogaman::Logger::GetLogger()->Log(LogLevel::LogWarning, x, __VA_ARGS__)
#define GM_LOG_ERROR(x, ...)   Gogaman::Logger::GetLogger()->Log(LogLevel::LogError, x, __VA_ARGS__)