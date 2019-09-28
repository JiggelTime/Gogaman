#pragma once

#include "Core/Base.h"
#include "Core/Logging/Log.h"

#define GM_RESOURCE_GENERATION_DATA_TYPE uint16_t

namespace Gogaman
{
	template<typename IndexDataType>
	struct ContainerID
	{
		GM_RESOURCE_GENERATION_DATA_TYPE generation;
		IndexDataType                    index;
	};

	template<uint32_t maxResourceCount, typename ResourceType, typename ID_IndexDataType>
	class StaticContainer
	{
	public:
		inline StaticContainer()
		{
			GM_STATIC_ASSERT(maxResourceCount <= std::numeric_limits<ID_IndexDataType>::max(), "Failed to construct resource container | Max resource count exceeds ID type limit")

			m_Resources.reserve(maxResourceCount);
		}

		inline StaticContainer(const StaticContainer &) = delete;
		inline StaticContainer(StaticContainer &&)      = default;

		inline ~StaticContainer() = default;

		inline StaticContainer &operator=(const StaticContainer &) = delete;
		inline StaticContainer &operator=(StaticContainer &&)      = default;

		template<typename ...ParameterTypes>
		inline ResourceType &Create(ID<ID_IndexDataType> &identifier, ParameterTypes &&...constructorParameters)
		{
			GM_ASSERT(m_Resources.size() < maxResourceCount, "Failed to add resource | Resource count exceeds %d", maxResourceCount - 1)
			
			identifier.index = m_Resources.size();

			ResourceType &resource = m_Resources.emplace_back(std::forward<ParameterTypes>(constructorParameters)...);
			return resource;
		}

		template<typename ...ParameterTypes>
		inline ID<ID_IndexDataType> Create(ParameterTypes &&...constructorParameters)
		{
			GM_ASSERT(m_Resources.size() < maxResourceCount, "Failed to add resource | Resource count exceeds %d", maxResourceCount - 1)

			ID<ID_IndexDataType> identifier;
			identifier.index = m_Resources.size();
			
			m_Resources.emplace_back(std::forward<ParameterTypes>(constructorParameters)...);
			return identifier;
		}

		inline constexpr ResourceType &Get(const ContainerID<ID_IndexDataType> identifier) { return Get(identifier.index); }

		inline constexpr ResourceType &Get(const ID_IndexDataType internalIndex)
		{
			GM_ASSERT(internalIndex < m_Resources.size(), "Failed to get resource | Invalid internal index")
			return m_Resources[internalIndex];
		}

		inline constexpr uint32_t GetResourceCount()    { return (uint32_t)m_Resources.size();  }
		inline constexpr uint32_t GetMaxResourceCount() { return maxResourceCount;              }

		inline typename std::vector<ResourceType>::iterator       begin()       { return m_Resources.begin(); }
		inline typename std::vector<ResourceType>::const_iterator begin() const { return m_Resources.begin(); }
	
		inline typename std::vector<ResourceType>::iterator       end()         { return m_Resources.end();   }
		inline typename std::vector<ResourceType>::const_iterator end()   const { return m_Resources.end();   }
	private:
		std::vector<ResourceType> m_Resources;
	};

	template<uint32_t maxResourceCount, typename ResourceType, typename ID_IndexDataType>
	class DynamicContainer
	{
	private:
		using ExternalID = ContainerID<ID_IndexDataType>;

		union InternalID
		{
			ID_IndexDataType index;
			ID_IndexDataType nextFreeExternalIndex;
		};
	public:
		inline DynamicContainer()
			: m_FreelistHead(0)
		{
			GM_STATIC_ASSERT(maxResourceCount <= std::numeric_limits<ID_IndexDataType>::max(), "Failed to construct resource container | Max resource count exceeds ID type limit")

			m_Generations.reserve(maxResourceCount);
			m_InternalIDs.reserve(maxResourceCount);
			m_Resources.reserve(maxResourceCount);

			for(ID_IndexDataType i = 0; i < maxResourceCount;)
			{
				m_Generations.emplace_back(0);

				InternalID internalID;
				internalID.nextFreeExternalIndex = ++i;
				m_InternalIDs.emplace_back(internalID);
			}
		}

		inline DynamicContainer(const DynamicContainer &) = delete;
		inline DynamicContainer(DynamicContainer &&)      = default;

		inline ~DynamicContainer() = default;

		inline DynamicContainer &operator=(const DynamicContainer &) = delete;
		inline DynamicContainer &operator=(DynamicContainer &&)      = default;

		template<typename ...ParameterTypes>
		inline ResourceType &Create(ExternalID &identifier, ParameterTypes &&...constructorParameters)
		{
			GM_ASSERT(m_Resources.size() < maxResourceCount, "Failed to add resource | Resource count exceeds %d", maxResourceCount - 1)
			
			ID_IndexDataType externalIndex     = m_FreelistHead;
			m_FreelistHead                     = m_InternalIDs[m_FreelistHead].nextFreeExternalIndex;
			m_InternalIDs[externalIndex].index = m_Resources.size();

			ResourceType &resource = m_Resources.emplace_back(std::forward<ParameterTypes>(constructorParameters)...);
			
			identifier.generation = m_Generations[externalIndex];
			identifier.index      = externalIndex;

			m_LastAddedExternalIndex = externalIndex;
			return resource;
		}

		template<typename ...ParameterTypes>
		inline ExternalID Create(ParameterTypes &&...constructorParameters)
		{
			GM_ASSERT(m_Resources.size() < maxResourceCount, "Failed to add resource | Resource count exceeds %d", maxResourceCount - 1)

			ID_IndexDataType externalIndex     = m_FreelistHead;
			m_FreelistHead                     = m_InternalIDs[m_FreelistHead].nextFreeExternalIndex;
			m_InternalIDs[externalIndex].index = m_Resources.size();
			
			m_Resources.emplace_back(std::forward<ParameterTypes>(constructorParameters)...);

			ExternalID identifier;
			identifier.generation = m_Generations[externalIndex];
			identifier.index      = externalIndex;

			m_LastAddedExternalIndex = externalIndex;
			return identifier;
		}

		inline constexpr bool IsIdentifierValid(const ExternalID identifier)
		{
			return identifier.generation == m_Generations[identifier.index];
		}

		inline constexpr ResourceType &Get(const ExternalID identifier)
		{
			GM_ASSERT(IsIdentifierValid(identifier), "Failed to get resource | Identifier's generation does not match current generation")
			return Get(m_InternalIDs[identifier.index].index);
		}

		inline constexpr ResourceType &Get(const ID_IndexDataType internalIndex)
		{
			GM_ASSERT(internalIndex < m_Resources.size(), "Failed to get resource | Invalid internal index")
			return m_Resources[internalIndex];
		}

		inline void Erase(const ExternalID identifier)
		{
			GM_ASSERT(m_Resources.size() > 0, "Failed to erase resource | Container is empty")
			GM_ASSERT(IsIdentifierValid(identifier),   "Failed to erase resource | Identifier's generation does not match current generation")

			ID_IndexDataType internalIndex = m_InternalIDs[identifier.index].index;

			Get(internalIndex) = m_Resources.back();
			m_Resources.pop_back();

			m_InternalIDs[m_LastAddedExternalIndex].index = internalIndex;
			m_Generations[identifier.index]++;

			m_InternalIDs[identifier.index].nextFreeExternalIndex = m_FreelistHead;
			m_FreelistHead                                        = identifier.index;
		}

		inline constexpr uint32_t GetResourceCount()    { return (uint32_t)m_Resources.size();  }
		inline constexpr uint32_t GetMaxResourceCount() { return maxResourceCount;              }

		inline typename std::vector<ResourceType>::iterator       begin()       { return m_Resources.begin(); }
		inline typename std::vector<ResourceType>::const_iterator begin() const { return m_Resources.begin(); }
	
		inline typename std::vector<ResourceType>::iterator       end()         { return m_Resources.end();   }
		inline typename std::vector<ResourceType>::const_iterator end()   const { return m_Resources.end();   }
	private:
		ID_IndexDataType                              m_FreelistHead;
		ID_IndexDataType                              m_LastAddedExternalIndex;
		std::vector<GM_RESOURCE_GENERATION_DATA_TYPE> m_Generations;
		std::vector<InternalID>                       m_InternalIDs;
		std::vector<ResourceType>                     m_Resources;
	};
}