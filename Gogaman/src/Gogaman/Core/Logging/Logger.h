#pragma once
#include "pch.h"

namespace Gogaman
{
	enum class LogLevel : uint8_t
	{
		Trace,
		Info,
		Warning,
		Error
	};

	class Logger
	{
	public:
		Logger(const std::string &name)
			: m_Name(name), m_LogLevel(LogLevel::Trace)
		{}

		~Logger() = default;

		void Log(const LogLevel level, const char *format, ...) const;

		inline void SetLogLevel(const LogLevel level) { m_LogLevel = level; }
		inline constexpr LogLevel GetLogLevel() const { return m_LogLevel; }
	protected:
		std::string m_Name;
		LogLevel    m_LogLevel;
	};
}