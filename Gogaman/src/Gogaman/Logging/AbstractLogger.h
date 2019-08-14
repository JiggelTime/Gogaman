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
		inline AbstractLogger(const std::string &name)
			: m_LogName(name), m_LogLevel(LogLevel::Trace)
		{}

		~AbstractLogger()
		{}

		inline virtual void Log(const LogLevel level, const char *format, ...) const = 0;

		inline void SetLogLevel(const LogLevel level) { m_LogLevel = level; }
		inline LogLevel GetLogLevel() const { return m_LogLevel; }
	protected:
		std::string m_LogName;
		LogLevel    m_LogLevel;
	};
}