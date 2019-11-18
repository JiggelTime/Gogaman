#pragma once

#include "Gogaman/Core/Base.h"

#define GM_INVALID_ID 0

namespace Gogaman
{
	template<typename ResourceIndexType>
	struct ResourceID
	{
		using Index      = ResourceIndexType;
		using Generation = uint16_t;

		Index      index = 0;
		#if GM_RHI_DEBUGGING_ENABLED
		Generation generation;
		#endif
	};

	template<typename ResourceType, typename ResourceIdentifierType, uint32_t resourceCountLimit>
	class ResourceContainer
	{
	private:
		using ID         = ResourceIdentifierType;
		using Index      = typename ID::Index;
		using Generation = typename ID::Generation;
	private:
		struct Element
		{
			Element()
			{}

			~Element() {}

			union
			{
				ResourceType resource;
				Index        nextFreeIndex = 0;
			};

			#if GM_RHI_DEBUGGING_ENABLED
				Generation generation = 0;
			#endif
		};
	public:
		ResourceContainer()
			: m_ElementCount(1)
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
				m_Elements[identifier.index].resource = std::move(ResourceType(constructorParameters...));
				m_ElementCount++;
				return identifier;
			}

			identifier.index = m_ElementCount++;
			m_Elements[identifier.index].resource = std::move(ResourceType(constructorParameters...));
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

			m_Elements[identifier.index].resource.~ResourceType();
			m_Elements[identifier.index].nextFreeIndex = m_Elements[0].nextFreeIndex;

			#if GM_RHI_DEBUGGING_ENABLED
				m_Elements[identifier.index].generation++;
			#endif

			m_Elements[0].nextFreeIndex = identifier.index;

			m_ElementCount--;
		}

		inline constexpr Index GetElementCount() const { return m_ElementCount; }
	private:
		inline void ValidateID(const ID identifier) const
		{
			#if GM_RHI_DEBUGGING_ENABLED
				GM_ASSERT(identifier.index != GM_INVALID_ID && identifier.index < m_ElementCount, "Invalid resource ID | Invalid index");
				GM_ASSERT(identifier.generation == m_Elements[identifier.index].generation,       "Invalid resource ID | ID generation does not match current generation");
			#endif
		}
	private:
		Index   m_ElementCount;
		Element m_Elements[resourceCountLimit];
	};
}