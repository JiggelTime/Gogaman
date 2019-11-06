#pragma once

#include "CRTP.h"

namespace Gogaman
{
	template<typename ImplementationType>
	class AbstractWindow : CRTP<ImplementationType, AbstractWindow>
	{
	public:
		//AbstractWindow(const AbstractWindow &) = delete;
		//AbstractWindow(AbstractWindow &&other) = default;

		//AbstractWindow &operator=(const AbstractWindow &) = delete;
		//AbstractWindow &operator=(AbstractWindow &&other) = default;

		inline void Update() { this->GetImplementation().Update(); }

		inline constexpr auto *GetNativeWindow() { return this->GetImplementation().GetNativeWindow(); }

		inline constexpr const char *GetTitle() const { return m_Title; }
		
		inline constexpr uint16_t GetWidth()  const { return m_Width;  }
		inline constexpr uint16_t GetHeight() const { return m_Height; }
		
		inline void EnableVerticalSynchronization()  { this->GetImplementation().EnableVerticalSynchronization(); }
		inline void DisableVerticalSynchronization() { this->GetImplementation().DisableVerticalSynchronization(); }
		inline constexpr bool IsVerticalSynchronizationEnabled() const { return m_IsVerticalSynchronizationEnabled; }
		
		static inline void Shutdown() { ImplementationType::Shutdown(); }
	protected:
		const char *m_Title;
		uint16_t    m_Width;
		uint16_t    m_Height;
		bool        m_IsVerticalSynchronizationEnabled;
	private:
		friend ImplementationType;
	private:
		AbstractWindow(const char *title, const uint16_t width, const uint16_t height)
			: m_Title(title), m_Width(width), m_Height(height), m_IsVerticalSynchronizationEnabled(true)
		{}

		~AbstractWindow() = default;
	};
}