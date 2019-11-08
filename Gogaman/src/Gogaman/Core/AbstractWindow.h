#pragma once

#include "CRTP.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	template<typename ImplementationType>
	class AbstractWindow : CRTP<ImplementationType, AbstractWindow>
	{
	public:
		AbstractWindow(const AbstractWindow &) = delete;
		AbstractWindow(AbstractWindow &&other) = default;

		AbstractWindow &operator=(const AbstractWindow &) = delete;
		AbstractWindow &operator=(AbstractWindow &&other) = default;

		inline void Update() { this->GetImplementation().Update(); }

		inline constexpr auto *GetNativeWindow() { return this->GetImplementation().GetNativeWindow(); }

		inline constexpr const char *GetTitle() const { return m_Title; }
		
		inline constexpr uint16_t GetWidth()  const { return m_Width;  }
		inline constexpr uint16_t GetHeight() const { return m_Height; }
		
		inline void SetVerticalSynchronization(const VerticalSynchronization verticalSynchronization) { this->GetImplementation().SetVerticalSynchronization(verticalSynchronization); }
		inline constexpr VerticalSynchronization GetVerticalSynchronization() const { return m_VerticalSynchronization; }
		
		static inline void Shutdown() { ImplementationType::Shutdown(); }
	protected:
		const char                   *m_Title;
		std::unique_ptr<RHI::Device>  m_Device;
		uint16_t                      m_Width, m_Height;
		VerticalSynchronization       m_VerticalSynchronization;
	private:
		friend ImplementationType;
	private:
		AbstractWindow(const char *title, const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization = VerticalSynchronization::Disabled)
			: m_Title(title), m_Width(width), m_Height(height), m_VerticalSynchronization(verticalSynchronization)
		{}

		~AbstractWindow() = default;
	};
}