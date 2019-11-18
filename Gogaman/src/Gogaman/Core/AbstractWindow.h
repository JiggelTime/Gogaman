#pragma once

#include "CRTP.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

#include "Gogaman/Events/EventListener.h"
#include "Gogaman/Events/EventDispatcher.h"
#include "Gogaman/Events/WindowEvent.h"

namespace Gogaman
{
	template<typename ImplementationType>
	class AbstractWindow : public CRTP<ImplementationType, AbstractWindow>, public EventListener
	{
	public:
		AbstractWindow(const AbstractWindow &) = delete;
		AbstractWindow(AbstractWindow &&other) = default;

		AbstractWindow &operator=(const AbstractWindow &) = delete;
		AbstractWindow &operator=(AbstractWindow &&other) = default;

		inline void Update() { this->GetImplementation().Update(); }

		inline virtual void OnEvent(Event &event) override
		{
			EventDispatcher dispatcher(event);
			dispatcher.Dispatch<WindowResizeEvent>(GM_BIND_EVENT_CALLBACK(AbstractWindow::OnWindowResize));
		}

		inline bool OnWindowResize(WindowResizeEvent &event)
		{
			Resize(event.GetWidth(), event.GetHeight());
			return true;
		}
		
		inline void Resize(const uint16_t width, const uint16_t height) { this->GetImplementation().Resize(width, height); }

		inline constexpr auto *GetNativeWindow() { return this->GetImplementation().GetNativeWindow(); }

		inline constexpr const char *GetTitle() const { return m_Title; }

		inline constexpr const RHI::Device &GetRenderHardwareInterfaceDevice() const { return *m_RenderHardwareInterfaceDevice.get(); }
		inline constexpr       RHI::Device &GetRenderHardwareInterfaceDevice()       { return *m_RenderHardwareInterfaceDevice.get(); }
		
		inline constexpr uint16_t GetWidth()  const { return m_Width;  }
		inline constexpr uint16_t GetHeight() const { return m_Height; }
		
		inline void SetVerticalSynchronization(const VerticalSynchronization verticalSynchronization) { this->GetImplementation().SetVerticalSynchronization(verticalSynchronization); }
		inline constexpr VerticalSynchronization GetVerticalSynchronization() const { return m_VerticalSynchronization; }		

		static inline void Shutdown() { ImplementationType::Shutdown(); }
	private:
		AbstractWindow(const char *title, const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization = VerticalSynchronization::Disabled)
			: m_Title(title), m_Width(width), m_Height(height), m_VerticalSynchronization(verticalSynchronization)
		{}

		~AbstractWindow() = default;
	protected:
		const char                   *m_Title;
		std::unique_ptr<RHI::Device>  m_RenderHardwareInterfaceDevice;
		uint16_t                      m_Width, m_Height;
		VerticalSynchronization       m_VerticalSynchronization;
	private:
		friend ImplementationType;
	};
}