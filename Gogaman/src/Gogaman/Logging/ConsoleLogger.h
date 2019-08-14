#pragma once

#include "AbstractLogger.h"
#include "Gogaman/Base.h"

namespace Gogaman
{
	class ConsoleLogger : public AbstractLogger
	{
	public:
		inline ConsoleLogger(const std::string &name)
			: AbstractLogger(name)
		{}

		~ConsoleLogger()
		{}

		inline virtual void Log(const LogLevel level, const char *format, ...) const override
		{
			if(level >= m_LogLevel)
			{
				char         *message = nullptr;
				unsigned int  messageLength = 0;
				va_list       args;
				va_start(args, format);
				messageLength = _vscprintf(format, args) + 1;
				message       = new char[messageLength];
				vsprintf_s(message, messageLength, format, args);

				std::ostringstream outputStringStream;
				outputStringStream << "[" << m_LogName << "] ";
				switch(level)
				{
				case LogLevel::Trace:
					outputStringStream << "TRACE:   ";
					break;
				case LogLevel::Info:
					outputStringStream << "INFO:    ";
					break;
				case LogLevel::Warning:
					outputStringStream << "WARNING: ";
					break;
				case LogLevel::Error:
					outputStringStream << "ERROR:   ";
					break;
				}

				outputStringStream << message << "\n";
				std::cout << outputStringStream.str();

				va_end(args);
				delete[] message;
			}
		}
	};
}