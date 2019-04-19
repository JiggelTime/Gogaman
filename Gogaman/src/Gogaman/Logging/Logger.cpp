#include "pch.h"
#include "Logger.h"

namespace Gogaman
{
	Logger::Logger(std::string name)
		: m_LogName(name)
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

			m_OutputStringStream << "[" << m_LogName << "] ";
			switch(level)
			{
			case LogLevel::Trace:
				m_OutputStringStream << "TRACE:   ";
				break;
			case LogLevel::Info:
				m_OutputStringStream << "INFO:    ";
				break;
			case LogLevel::Warning:
				m_OutputStringStream << "WARNING: ";
				break;
			case LogLevel::Error:
				m_OutputStringStream << "ERROR:   ";
				break;
			}

			m_OutputStringStream << message << "\n";
			std::cout << m_OutputStringStream.str();

			va_end(args);
			delete[] message;
		}
	}
}