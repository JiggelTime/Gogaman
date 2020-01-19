#pragma once

#include "Fence.h"
#include "pch.h"

namespace Gogaman
{
	namespace RHI
	{
		class LinearFence : public Fence
		{
		public:
			LinearFence(const uint64_t count)
				: Fence(count), m_Index(0)
			{
				m_PreviousValues.resize(count);
			}

			~LinearFence() = default;

			inline void Next()
			{
				m_PreviousValues[m_Index] = m_DeviceValue;

				m_Index = (m_Index + 1) % m_PreviousValues.size();

				SynchronizeHost(m_PreviousValues[m_Index]);
			}
		private:
			std::vector<uint64_t> m_PreviousValues;
			uint64_t              m_Index;
		};
	}
}