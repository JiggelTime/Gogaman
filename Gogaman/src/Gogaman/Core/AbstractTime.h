#pragma once

#include "Gogaman/CRTP.h"

namespace Gogaman
{
	template<typename ImplementationType>
	class AbstractTime : public CRTP<ImplementationType, AbstractTime>
	{
	public:
		static inline void Update()
		{
			float time     = GetTime();
			m_DeltaTime    = time - m_PreviousTime;
			m_PreviousTime = time;

			m_FrameCount++;
		}

		static inline const float    GetTime()       { return ImplementationType::GetTime(); }
		static inline const float    GetDeltaTime()  { return m_DeltaTime;                   }
		static inline const uint32_t GetFrameCount() { return m_FrameCount;                  }
	private:
		static inline float    m_PreviousTime = 0.0f;
		static inline float    m_DeltaTime    = 0.0f;
		static inline uint32_t m_FrameCount   = 0;
	};
}