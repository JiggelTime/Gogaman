#pragma once

#include "Core/Base.h"
#include "Core/Logging/Log.h"

#define GM_ENABLE_CONTAINER_ID_VALIDATION 0

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

	template<uint32_t maxElementCount, typename ResourceType, typename ID_IndexDataType>
	class StaticContainer
	{
	public:
		inline StaticContainer()
		{
			GM_STATIC_ASSERT(maxElementCount <= std::numeric_limits<ID_IndexDataType>::max(), "Failed to construct container | Max element count exceeds ID type limit")

			m_Resources.reserve(maxElementCount);
		}

		inline StaticContainer(const StaticContainer &) = delete;
		inline StaticContainer(StaticContainer &&)      = default;

		inline ~StaticContainer() = default;

		inline StaticContainer &operator=(const StaticContainer &) = delete;
		inline StaticContainer &operator=(StaticContainer &&)      = default;

		template<typename ...ParameterTypes>
		inline ResourceType &Create(ContainerID<ID_IndexDataType> &identifier, ParameterTypes &&...constructorParameters)
		{
			GM_ASSERT(m_Resources.size() < maxElementCount, "Failed to add element | Resource count exceeds %d", maxElementCount - 1)
			
			identifier.index = m_Resources.size();

			ResourceType &element = m_Resources.emplace_back(std::forward<ParameterTypes>(constructorParameters)...);
			return element;
		}

		template<typename ...ParameterTypes>
		inline ContainerID<ID_IndexDataType> Create(ParameterTypes &&...constructorParameters)
		{
			GM_ASSERT(m_Resources.size() < maxElementCount, "Failed to add element | Resource count exceeds %d", maxElementCount - 1)

			ContainerID<ID_IndexDataType> identifier;
			identifier.index = m_Resources.size();
			
			m_Resources.emplace_back(std::forward<ParameterTypes>(constructorParameters)...);
			return identifier;
		}

		inline constexpr ResourceType &Get(const ContainerID<ID_IndexDataType> identifier) { return Get(identifier.index); }

		inline constexpr ResourceType &Get(const ID_IndexDataType internalIndex)
		{
			GM_ASSERT(internalIndex < m_Resources.size(), "Failed to get element | Invalid internal index")
			return m_Resources[internalIndex];
		}

		inline constexpr uint32_t GetResourceCount()    { return (uint32_t)m_Resources.size();  }
		inline constexpr uint32_t GetMaxResourceCount() { return maxElementCount;              }

		inline typename std::vector<ResourceType>::iterator       begin()       { return m_Resources.begin(); }
		inline typename std::vector<ResourceType>::const_iterator begin() const { return m_Resources.begin(); }
	
		inline typename std::vector<ResourceType>::iterator       end()         { return m_Resources.end();   }
		inline typename std::vector<ResourceType>::const_iterator end()   const { return m_Resources.end();   }
	private:
		std::vector<ResourceType> m_Resources;
	};

	template<uint32_t maxElementCount, typename ResourceType, typename ID_IndexDataType>
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
			GM_STATIC_ASSERT(maxElementCount <= std::numeric_limits<ID_IndexDataType>::max(), "Failed to construct container | Max element count exceeds ID type limit")

			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				m_Generations.reserve(maxElementCount);
			#endif
			m_InternalIDs.reserve(maxElementCount);
			m_Resources.reserve(maxElementCount);

			for(ID_IndexDataType i = 0; i < maxElementCount;)
			{
				#if GM_ENABLE_CONTAINER_ID_VALIDATION
					m_Generations.emplace_back(0);
				#endif

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
			GM_ASSERT(m_Resources.size() < maxElementCount, "Failed to add element | Resource count exceeds %d", maxElementCount - 1)
			
			ID_IndexDataType externalIndex     = m_FreelistHead;
			m_FreelistHead                     = m_InternalIDs[m_FreelistHead].nextFreeExternalIndex;
			m_InternalIDs[externalIndex].index = m_Resources.size();

			ResourceType &element = m_Resources.emplace_back(std::forward<ParameterTypes>(constructorParameters)...);
			
			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				identifier.generation = m_Generations[externalIndex];
			#endif
			identifier.index      = externalIndex;

			m_LastAddedExternalIndex = externalIndex;
			return element;
		}

		template<typename ...ParameterTypes>
		inline ExternalID Create(ParameterTypes &&...constructorParameters)
		{
			GM_ASSERT(m_Resources.size() < maxElementCount, "Failed to add element | Resource count exceeds %d", maxElementCount - 1)

			ID_IndexDataType externalIndex     = m_FreelistHead;
			m_FreelistHead                     = m_InternalIDs[m_FreelistHead].nextFreeExternalIndex;
			m_InternalIDs[externalIndex].index = m_Resources.size();
			
			m_Resources.emplace_back(std::forward<ParameterTypes>(constructorParameters)...);

			ExternalID identifier;
			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				identifier.generation = m_Generations[externalIndex];
			#endif
			identifier.index      = externalIndex;

			m_LastAddedExternalIndex = externalIndex;
			return identifier;
		}

		inline constexpr bool IsIdentifierValid(const ExternalID identifier) const
		{
			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				return identifier.generation == m_Generations[identifier.index];
			#else
				GM_ASSERT(false, "Failed to validate ID | Container ID validation is disabled")
			#endif
		}

		inline constexpr ResourceType &Get(const ExternalID identifier)
		{
			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				GM_ASSERT(IsIdentifierValid(identifier), "Failed to get element | Identifier's generation does not match current generation")
			#endif
			return Get(m_InternalIDs[identifier.index].index);
		}

		inline constexpr ResourceType &Get(const ID_IndexDataType internalIndex)
		{
			GM_ASSERT(internalIndex < m_Resources.size(), "Failed to get element | Invalid internal index")
			return m_Resources[internalIndex];
		}

		inline void Erase(const ExternalID identifier)
		{
			GM_ASSERT(m_Resources.size() > 0,            "Failed to erase element | Container is empty")
			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				GM_ASSERT(IsIdentifierValid(identifier), "Failed to erase element | Invalid ID")
			#endif

			ID_IndexDataType internalIndex = m_InternalIDs[identifier.index].index;

			Get(internalIndex) = m_Resources.back();
			m_Resources.pop_back();

			m_InternalIDs[m_LastAddedExternalIndex].index = internalIndex;
			#if GM_ENABLE_CONTAINER_ID_VALIDATION
				m_Generations[identifier.index]++;
			#endif

			m_InternalIDs[identifier.index].nextFreeExternalIndex = m_FreelistHead;
			m_FreelistHead                                        = identifier.index;
		}

		inline constexpr uint32_t GetResourceCount()    { return (uint32_t)m_Resources.size();  }
		inline constexpr uint32_t GetMaxResourceCount() { return maxElementCount;              }

		inline typename std::vector<ResourceType>::iterator       begin()       { return m_Resources.begin(); }
		inline typename std::vector<ResourceType>::const_iterator begin() const { return m_Resources.begin(); }
	
		inline typename std::vector<ResourceType>::iterator       end()         { return m_Resources.end();   }
		inline typename std::vector<ResourceType>::const_iterator end()   const { return m_Resources.end();   }
	private:
		ID_IndexDataType          m_FreelistHead;
		ID_IndexDataType          m_LastAddedExternalIndex;
		#if GM_ENABLE_CONTAINER_ID_VALIDATION
			std::vector<uint16_t> m_Generations;
		#endif
		std::vector<InternalID>   m_InternalIDs;
		std::vector<ResourceType> m_Resources;
	};
}