#pragma once

#include "Core/Base.h"
#include "Core/Logging/Log.h"

#define GM_ENABLE_CONTAINER_ID_VALIDATION 1

namespace Gogaman
{
	template<typename IndexDataType>
	struct ContainerID
	{
		#if GM_ENABLE_CONTAINER_ID_VALIDATION
			uint16_t  generation;
		#endif
		IndexDataType index;
	};

	template<uint32_t maxElementCount, typename ResourceType, typename IdentifierIndexDataType>
	class StableArray
	{
	public:
		StableArray()
		{
			GM_STATIC_ASSERT(maxElementCount <= std::numeric_limits<IdentifierIndexDataType>::max(), "Failed to construct container | Max element count exceeds ID type limit");

			m_Resources.reserve(maxElementCount);
		}

		StableArray(const StableArray &) = delete;
		StableArray(StableArray &&)      = default;

		~StableArray() = default;

		inline StableArray &operator=(const StableArray &) = delete;
		inline StableArray &operator=(StableArray &&)      = default;

		template<typename ...ParameterTypes>
		inline ResourceType &Create(ContainerID<IdentifierIndexDataType> &identifier, ParameterTypes &&...constructorParameters)
		{
			GM_ASSERT(m_Resources.size() < maxElementCount, "Failed to add element | Resource count exceeds %d", maxElementCount - 1);
			
			identifier.index = m_Resources.size();

			ResourceType &element = m_Resources.emplace_back(std::forward<ParameterTypes>(constructorParameters)...);
			return element;
		}

		template<typename ...ParameterTypes>
		inline ContainerID<IdentifierIndexDataType> Create(ParameterTypes &&...constructorParameters)
		{
			GM_ASSERT(m_Resources.size() < maxElementCount, "Failed to add element | Resource count exceeds %d", maxElementCount - 1);

			ContainerID<IdentifierIndexDataType> identifier;
			identifier.index = m_Resources.size();
			
			m_Resources.emplace_back(std::forward<ParameterTypes>(constructorParameters)...);
			return identifier;
		}

		inline constexpr ResourceType &Get(const ContainerID<IdentifierIndexDataType> identifier) { return Get(identifier.index); }

		inline constexpr ResourceType &Get(const IdentifierIndexDataType internalIndex)
		{
			GM_ASSERT(internalIndex < m_Resources.size(), "Failed to get element | Invalid internal index");
			return m_Resources[internalIndex];
		}

		inline constexpr uint32_t GetResourceCount() { return (uint32_t)m_Resources.size(); }
		inline constexpr uint32_t GetMaxResourceCount() { return maxElementCount; }

		inline typename std::vector<ResourceType>::iterator       begin()       { return m_Resources.begin(); }
		inline typename std::vector<ResourceType>::const_iterator begin() const { return m_Resources.begin(); }
	
		inline typename std::vector<ResourceType>::iterator       end()         { return m_Resources.end();   }
		inline typename std::vector<ResourceType>::const_iterator end()   const { return m_Resources.end();   }
	private:
		std::vector<ResourceType> m_Resources;
	};

	template<uint32_t maxElementCount, typename ResourceType, typename IdentifierIndexDataType>
	class DynamicStableArray
	{
	private:
		using ExternalID = ContainerID<IdentifierIndexDataType>;

		union InternalID
		{
			IdentifierIndexDataType index;
			IdentifierIndexDataType nextFreeExternalIndex;
		};
	public:
		DynamicStableArray()
			: m_FreelistHead(0)
		{
			GM_STATIC_ASSERT(maxElementCount <= std::numeric_limits<IdentifierIndexDataType>::max(), "Failed to construct container | Max element count exceeds ID type limit");

			m_InternalIndexExternalIndices.resize(maxElementCount);
			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				m_Generations.reserve(maxElementCount);
			#endif
			m_InternalIDs.reserve(maxElementCount);
			m_Resources.reserve(maxElementCount);

			for(IdentifierIndexDataType i = 0; i < maxElementCount;)
			{
				#if GM_ENABLE_CONTAINER_ID_VALIDATION
					m_Generations.emplace_back(0);
				#endif

				InternalID internalID;
				internalID.nextFreeExternalIndex = ++i;
				m_InternalIDs.emplace_back(internalID);
			}
		}

		DynamicStableArray(const DynamicStableArray &) = delete;
		DynamicStableArray(DynamicStableArray &&)      = default;

		~DynamicStableArray() = default;

		inline DynamicStableArray &operator=(const DynamicStableArray &) = delete;
		inline DynamicStableArray &operator=(DynamicStableArray &&)      = default;

		template<typename ...ParameterTypes>
		inline ResourceType &Create(ExternalID &identifier, ParameterTypes &&...constructorParameters)
		{
			GM_ASSERT(m_Resources.size() < maxElementCount, "Failed to add element | Resource count exceeds %d", maxElementCount - 1);
			
			IdentifierIndexDataType externalIndex = m_FreelistHead;
			m_FreelistHead = m_InternalIDs[m_FreelistHead].nextFreeExternalIndex;

			IdentifierIndexDataType internalIndex = m_Resources.size();
			m_InternalIDs[externalIndex].index = internalIndex;
			m_InternalIndexExternalIndices[internalIndex] = externalIndex;

			ResourceType &element = m_Resources.emplace_back(std::forward<ParameterTypes>(constructorParameters)...);
			
			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				identifier.generation = m_Generations[externalIndex];
			#endif
			identifier.index = externalIndex;

			return element;
		}

		template<typename ...ParameterTypes>
		inline ExternalID Create(ParameterTypes &&...constructorParameters)
		{
			GM_ASSERT(m_Resources.size() < maxElementCount, "Failed to add element | Resource count exceeds %d", maxElementCount - 1);

			IdentifierIndexDataType externalIndex = m_FreelistHead;
			m_FreelistHead = m_InternalIDs[m_FreelistHead].nextFreeExternalIndex;

			IdentifierIndexDataType internalIndex = m_Resources.size();
			m_InternalIDs[externalIndex].index = internalIndex;
			
			m_Resources.emplace_back(std::forward<ParameterTypes>(constructorParameters)...);

			ExternalID identifier;
			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				identifier.generation = m_Generations[externalIndex];
			#endif
			identifier.index      = externalIndex;

			m_InternalIndexExternalIndices[internalIndex] = externalIndex;
			return identifier;
		}

		inline constexpr bool IsIdentifierValid(const ExternalID identifier) const
		{
			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				return identifier.generation == m_Generations[identifier.index];
			#else
				GM_ASSERT(false, "Failed to validate ID | Container ID validation is disabled");
			#endif
		}

		inline constexpr ResourceType &Get(const ExternalID identifier)
		{
			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				GM_ASSERT(IsIdentifierValid(identifier), "Failed to get element | Identifier's generation does not match current generation");
			#endif
			return Get(m_InternalIDs[identifier.index].index);
		}

		inline constexpr ResourceType &Get(const IdentifierIndexDataType internalIndex)
		{
			GM_ASSERT(internalIndex < m_Resources.size(), "Failed to get element | Invalid internal index");
			return m_Resources[internalIndex];
		}

		inline void Erase(const ExternalID identifier)
		{
			GM_ASSERT(m_Resources.size() > 0,            "Failed to erase element | Container is empty");
			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				GM_ASSERT(IsIdentifierValid(identifier), "Failed to erase element | Invalid ID");
			#endif

			IdentifierIndexDataType internalIndex = m_InternalIDs[identifier.index].index;

			Get(internalIndex) = std::move(m_Resources.back());
			m_Resources.pop_back();

			IdentifierIndexDataType lastElementExternalIndex = m_InternalIndexExternalIndices[m_Resources.size()];
			m_InternalIDs[lastElementExternalIndex].index = internalIndex;
			m_InternalIndexExternalIndices[internalIndex] = lastElementExternalIndex;
			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				m_Generations[identifier.index]++;
			#endif

			m_InternalIDs[identifier.index].nextFreeExternalIndex = m_FreelistHead;
			m_FreelistHead = identifier.index;
		}

		inline constexpr uint32_t GetResourceCount() { return (uint32_t)m_Resources.size(); }
		inline constexpr uint32_t GetMaxResourceCount() { return maxElementCount; }

		inline typename std::vector<ResourceType>::iterator       begin()       { return m_Resources.begin(); }
		inline typename std::vector<ResourceType>::const_iterator begin() const { return m_Resources.begin(); }
	
		inline typename std::vector<ResourceType>::iterator       end()         { return m_Resources.end();   }
		inline typename std::vector<ResourceType>::const_iterator end()   const { return m_Resources.end();   }
	private:
		IdentifierIndexDataType              m_FreelistHead;
		std::vector<IdentifierIndexDataType> m_InternalIndexExternalIndices;
		#if GM_ENABLE_CONTAINER_ID_VALIDATION
			std::vector<uint16_t>     m_Generations;
		#endif
		std::vector<InternalID>       m_InternalIDs;
		std::vector<ResourceType>     m_Resources;
	};
}