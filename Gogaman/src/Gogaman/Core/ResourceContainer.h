#pragma once

#include "Gogaman/Core/Base.h"

#define GM_INVALID_ID 0

namespace Gogaman
{
	template<typename ResourceType, typename ResourceIndexType, uint32_t resourceCountLimit>
	class ResourceContainer
	{
	private:
		using Index      = ResourceIndexType;
		using Generation = uint16_t;
	public:
		struct ID
		{
			Index          index = 0;
			#if GM_RHI_DEBUGGING_ENABLED
				Generation generation;
			#endif
		};
	private:
		struct Element
		{
			union
			{
				ResourceType resource;
				Index        nextFreeIndex;
			};

			#if GM_RHI_DEBUGGING_ENABLED
				Generation generation = 0;
			#endif
		};
	public:
		ResourceContainer()
			: m_ElementCount(0)
		{}

		~ResourceContainer() = default;

		template<typename ...ParameterTypes>
		inline ID Create(ParameterTypes &&...constructorParameters)
		{
			ID identifier;
			identifier.index            = m_Elements[0].nextFreeIndex;
			m_Elements[0].nextFreeIndex = m_Elements[identifier.index].nextFreeIndex;
			
			#if GM_RHI_DEBUGGING_ENABLED
				identifier.generation = m_Elements[identifier.index].generation;
			#endif

			if(identifier.index)
			{
				m_Elements[identifier.index].resource = std::move(ResourceType(constructorParameters));
				return identifier;
			}

			identifier.index = m_ElementCount++;
			m_Elements[identifier.index].resource = std::move(ResourceType(constructorParameters));
			return identifier;
		}

		inline constexpr const ResourceType &Get(const ID identifier) const
		{
			ValidateID(identifier);
			return m_Elements[identifier.index].resource;
		}

		inline constexpr ResourceType &Get(const ID identifier)
		{
			ValidateID(identifier);
			return m_Elements[identifier.index].resource;
		}

		inline void Destroy(const ID identifier)
		{
			ValidateID(identifier);

			//m_Elements[identifier.index].resource.~ResourceType();
			m_Elements[identifier.index].nextFreeIndex = m_Elements[0].nextFreeIndex;

			#if GM_RHI_DEBUGGING_ENABLED
				m_Elements[identifier.index].generation++;
			#endif

			m_Elements[0].nextFreeIndex = identifier.index;
		}
	private:
		inline void ValidateID(const ID identifier) const
		{
			#if GM_RHI_DEBUGGING_ENABLED
				GM_ASSERT(identifier.index != 0 && identifier.index < m_ElementCount,       "Invalid resource ID | Invalid index");
				GM_ASSERT(identifier.generation == m_Elements[identifier.index].generation, "Invalid resource ID | ID generation does not match current generation");
			#endif
		}
	private:
		Index   m_ElementCount;
		Element m_Elements[resourceCountLimit];
	};
}