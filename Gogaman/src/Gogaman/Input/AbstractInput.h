#pragma once

#include "Gogaman/Core/CRTP.h"

#include "KeyCodes.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	template<typename ImplementationType>
	class AbstractInput : public CRTP<ImplementationType, AbstractInput>
	{
	public:
		static inline void Update()
		{
			glm::vec2 mousePosition = GetMousePosition();
			m_DeltaMousePosition    = mousePosition - m_PreviousMousePosition;
			m_PreviousMousePosition = mousePosition;
		}

		//Keyboard
		static inline bool IsKeyPressed(const int scancode) { return ImplementationType::IsKeyPressed(scancode); }

		//Mouse
		static inline bool IsMouseButtonPressed(const int scancode) { return ImplementationType::IsMouseButtonPressed(scancode); }

		static inline const glm::vec2 &GetMousePosition()         { return ImplementationType::GetMousePosition(); }
		static inline const glm::vec2 &GetPreviousMousePosition() { return m_PreviousMousePosition;                }
		static inline const glm::vec2 &GetDeltaMousePosition()    { return m_DeltaMousePosition;                   }
	private:
		friend ImplementationType;
	private:
		AbstractInput()  = delete;
		~AbstractInput() = delete;
	private:
		static inline glm::vec2 m_PreviousMousePosition = glm::vec2(0.0f);
		static inline glm::vec2 m_DeltaMousePosition    = glm::vec2(0.0f);
	};
}