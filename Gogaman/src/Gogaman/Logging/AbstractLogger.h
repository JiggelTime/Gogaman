#pragma once

#include "pch.h"
#include "Gogaman/Base.h"

namespace Gogaman
{
	enum class LogLevel
	{
		Trace, Info, Warning, Error
	};

	class AbstractLogger
	{
	public:
		AbstractLogger(std::string name);
		~AbstractLogger();

		virtual void Log(const LogLevel level, const char *format, ...) = 0;

		inline void SetLogLevel(LogLevel level) { m_LogLevel = level; }
		inline LogLevel GetLogLevel() { return m_LogLevel; }
	protected:
		std::string m_LogName;
		LogLevel    m_LogLevel;
	};
}