#pragma once
#pragma warning(disable:4996)

#include "AbstractLogger.h"
#include "Gogaman/Base.h"
#include "Gogaman/Config.h"

namespace Gogaman
{
	class FileLogger : public AbstractLogger
	{
	public:
		inline FileLogger(const std::string &name)
			: AbstractLogger(name)
		{}

		~FileLogger()
		{}

		inline virtual void Log(const LogLevel level, const char *format, ...) const override
		{
			if(level >= m_LogLevel)
			{
				FILE *file = fopen(GM_CONFIG.logFilepath.c_str(), "a");
				if(file)
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

					outputStringStream << message;
					fputs(outputStringStream.str().c_str(), file);
					fputs("\n", file);

					va_end(args);
					delete[] message;
					fclose(file);
				}
				else
				{
					std::cerr << "Failed to open log file | Location: " << GM_CONFIG.logFilepath << std::endl;
					exit(1);
				}
			}
		}
	};
}