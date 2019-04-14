#include "pch.h"
#include "Logger.h"

namespace Gogaman
{
	Logger* Logger::s_Instance = nullptr;
	LogLevel Logger::m_LogLevel = LogLevel::LogInfo;

	Logger::Logger()
	{
	}

	Logger::~Logger()
	{
	}

	void Logger::Log(const LogLevel level, const char *format, ...)
	{
		if(level >= m_LogLevel)
		{
			char *message = nullptr;
			unsigned int messageLength = 0;
			va_list args;
			va_start(args, format);
			messageLength = _vscprintf(format, args) + 1;
			message = new char[messageLength];
			vsprintf_s(message, messageLength, format, args);
			std::ostringstream oss;
			switch(level)
			{
			case LogLevel::LogInfo:
				oss << "INFO:    ";
				break;
			case LogLevel::LogWarning:
				oss << "WARNING: ";
				break;
			case LogLevel::LogError:
				oss << "ERROR:   ";
				break;
			}

			oss << message << "\n";
			std::cout << oss.str();
			va_end(args);
		}
	}

	void Logger::Log(const LogLevel level, const std::string &message)
	{
		std::ostringstream oss;

		if(level >= m_LogLevel)
		{
			switch(level)
			{
			case LogLevel::LogInfo:
				oss << "INFO:    ";
				break;
			case LogLevel::LogWarning:
				oss << "WARNING: ";
				break;
			case LogLevel::LogError:
				oss << "ERROR:   ";
				break;
			}

			oss << message << "\n";
			std::cout << oss.str();
		}
	}

	Logger *Logger::GetLogger()
	{
		if(s_Instance == nullptr)
			s_Instance = new Logger();

		return s_Instance;
	}

	void Logger::SetLogLevel(LogLevel level)
	{
		m_LogLevel = level;
	}

	LogLevel Logger::GetLogLevel()
	{
		return m_LogLevel;
	}
}