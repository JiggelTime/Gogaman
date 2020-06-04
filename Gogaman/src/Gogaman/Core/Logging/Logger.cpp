#include "pch.h"
#include "Logger.h"

//TODO: These options should be in config (gives "cant open glm.hpp no file or directory" error in #include glm.hpp in config.h
//#include "Gogaman/Core/Config.h"
#define GM_LOG_TO_FILE  0
#define GM_LOG_FILEPATH "Gogaman.log"

namespace Gogaman
{
	void Logger::Log(const LogLevel level, const char *format, ...) const
	{
		if(level >= m_LogLevel)
		{
			unsigned int  messageLength = 0;
			va_list       args;
			va_start(args, format);
			messageLength = _vscprintf(format, args) + 1;
			char *message = new char[messageLength];
			vsprintf_s(message, messageLength, format, args);

			std::ostringstream output;
			output << "[" << m_Name << "] ";
			switch(level)
			{
			case LogLevel::Trace:
				output << "TRACE:   ";
				break;
			case LogLevel::Info:
				output << "INFO:    ";
				break;
			case LogLevel::Warning:
				output << "WARNING: ";
				break;
			case LogLevel::Error:
				output << "ERROR:   ";
				break;
			}

			output << message << "\n";

			std::cout << output.str();

			#if GM_LOG_TO_FILE
				FILE *file = fopen(GM_LOG_FILEPATH, "a");
				if(file)
				{
					fputs(output.str().c_str(), file);
					fclose(file);
				}
				else
				{
					std::cerr << "Failed to open log file | Location: " << GM_LOG_FILEPATH << std::endl;
					exit(1);
				}
			#endif

			va_end(args);
			delete[] message;
		}
	}
}