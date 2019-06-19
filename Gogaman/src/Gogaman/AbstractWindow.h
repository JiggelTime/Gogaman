#pragma once

#include "Gogaman/CRTP.h"
#include "Gogaman/Graphics/RenderDeviceContext.h"

namespace Gogaman
{
	template<typename WindowType>
	class AbstractWindow : CRTP<WindowType, AbstractWindow>
	{
	public:
		AbstractWindow(const char *title, const int width, const int height)
			: m_Title(title), m_Width(width), m_Height(height), m_VerticalSynchronization(true)
		{}

		virtual ~AbstractWindow()
		{}

		static AbstractWindow *Create(const char *title, const int width, const int height);

		virtual void Update() = 0;
		static  void Shutdown();

		virtual void *GetNativeWindow() const = 0;

		virtual void EnableVerticalSync()  = 0;
		virtual void DisableVerticalSync() = 0;
		inline  bool IsVerticalSynchronizationEnabled() const { return m_VerticalSynchronization; }

		inline const char *GetTitle() const { return m_Title; }

		inline int GetWidth()  const { return m_Width;  }
		inline int GetHeight() const { return m_Height; }
	protected:
		const char *m_Title;

		int  m_Width;
		int  m_Height;

		bool m_VerticalSynchronization;

		std::unique_ptr<RenderDeviceContext> m_RenderDeviceContext;
	};
}