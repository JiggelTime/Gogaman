#pragma once

#include "FlexData.h"

namespace Gogaman
{
	namespace Tools
	{
		class Importer
		{
		public:
			virtual FlexData::FlexData Import(const char *filepath) = 0;
		protected:
			FlexData::FlexData m_FlexData;
		};
	}
}