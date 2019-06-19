#pragma once

#include "pch.h"
#include "AbstractLogger.h"
#include "Gogaman/Base.h"

namespace Gogaman
{
	class ConsoleLogger : public AbstractLogger
	{
	public:
		ConsoleLogger(std::string name);
		~ConsoleLogger();

		virtual void Log(const LogLevel level, const char* format, ...) override;
	};
}