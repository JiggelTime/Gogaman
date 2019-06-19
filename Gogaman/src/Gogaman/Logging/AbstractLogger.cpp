#include "pch.h"
#include "AbstractLogger.h"

namespace Gogaman
{
	AbstractLogger::AbstractLogger(std::string name)
		: m_LogName(name), m_LogLevel(LogLevel::Trace)
	{
	}

	AbstractLogger::~AbstractLogger()
	{
	}
}