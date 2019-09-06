#pragma once

#include "Core/Base.h"
#include "Logging/Log.h"

namespace Gogaman
{
	namespace Resource
	{
		template<typename UnsignedIntegerType, uint8_t generationBits, uint8_t indexBits>
		struct ID
		{
			//TODO: Make compatible with machines with non-standard byte size
			GM_STATIC_ASSERT(generationBits + indexBits == sizeof(UnsignedIntegerType) * 8, "Failed to construct resource identifier | Generation + index bit count does not match data type")

			UnsignedIntegerType generation : generationBits;
			UnsignedIntegerType index      : indexBits;
		};

		template<uint32_t maxResourceCount, typename ResourceType, typename ID_UnsignedIntegerType, uint8_t identifierGenerationBits, uint8_t identifierIndexBits>
		class Container
		{
		private:
			using ExternalID = ID<ID_UnsignedIntegerType, identifierGenerationBits, identifierIndexBits>;

			//using InternalIdentifier = std::variant<Identifier<ID_UnsignedIntegerType, identifierGenerationBits, identifierIndexBits>, ID_UnsignedIntegerType>;
			union InternalID
			{
				ID<ID_UnsignedIntegerType, identifierGenerationBits, identifierIndexBits> internalID;
				ID_UnsignedIntegerType                                                    nextFreeExternalIndex;
			};
		public:
			inline Container()
				: m_FreelistHead(0)
			{
				m_InternalIDs.reserve(maxResourceCount);
				m_Resources.reserve(maxResourceCount);

				for(uint32_t i = 0; i < maxResourceCount;)
				{
					InternalID internalID;
					internalID.nextFreeExternalIndex = ++i;
					m_InternalIDs.emplace_back(internalID);
				}
			}

			inline Container(const Container &) = delete;
			inline Container(Container &&)      = default;

			inline ~Container() = default;

			inline Container &operator=(const Container &) = delete;
			inline Container &operator=(Container &&)      = default;

			template<typename ...ParameterTypes>
			inline ExternalID Create(ParameterTypes &&...constructorParameters)
			{
				GM_ASSERT(m_Resources.size() < maxResourceCount, "Failed to add resource | Resource count exceeds %d", maxResourceCount - 1)

				ID_UnsignedIntegerType internalIndex = m_Resources.size();
				m_Resources.emplace_back(std::forward<ParameterTypes>(constructorParameters)...);

				ID_UnsignedIntegerType externalIndex = m_FreelistHead;
				m_FreelistHead                       = m_InternalIDs[m_FreelistHead].nextFreeExternalIndex;
				auto &internalID                     = m_InternalIDs[externalIndex].internalID;
				internalID.index                     = internalIndex;

				ExternalID externalID;
				externalID.generation = internalID.generation;
				externalID.index      = externalIndex;

				m_LastAddedExternalIndex = externalIndex;
				return externalID;
			}

			inline constexpr ResourceType &Get(const ExternalID identifier)
			{
				GM_ASSERT(identifier.generation == m_InternalIDs[identifier.index].internalID.generation, "Failed to get resource | Identifier's generation does not match current generation")
				return Get(m_InternalIDs[identifier.index].internalID.index);
			}

			inline constexpr ResourceType &Get(const ID_UnsignedIntegerType internalIndex)
			{
				GM_ASSERT(internalIndex < m_Resources.size(), "Failed to get resource | Invalid internal index")
				return m_Resources[internalIndex];
			}

			inline void Erase(const ExternalID identifier)
			{
				GM_ASSERT(m_Resources.size() > 0,                                                         "Failed to erase resource | Container is empty")
				GM_ASSERT(identifier.generation == m_InternalIDs[identifier.index].internalID.generation, "Failed to erase resource | Identifier's generation does not match current generation")

				auto &internalID      = m_InternalIDs[identifier.index].internalID;
				Get(internalID.index) = m_Resources.back();
				m_Resources.pop_back();

				m_InternalIDs[m_LastAddedExternalIndex].internalID.index = internalID.index;
				internalID.generation++;

				m_InternalIDs[identifier.index].nextFreeExternalIndex = m_FreelistHead;
				m_FreelistHead                                        = identifier.index;
			}

			inline constexpr ID_UnsignedIntegerType GetResourceCount()    { return m_Resources.size();  }
			inline constexpr uint32_t               GetMaxResourceCount() { return maxResourceCount;    }

			inline typename std::vector<ResourceType>::iterator       begin()       { return m_Resources.begin(); }
			inline typename std::vector<ResourceType>::const_iterator begin() const { return m_Resources.begin(); }
		
			inline typename std::vector<ResourceType>::iterator       end()         { return m_Resources.end();   }
			inline typename std::vector<ResourceType>::const_iterator end()   const { return m_Resources.end();   }
		private:
			ID_UnsignedIntegerType    m_FreelistHead;
			ID_UnsignedIntegerType    m_LastAddedExternalIndex;
			std::vector<InternalID>   m_InternalIDs;
			std::vector<ResourceType> m_Resources;
		};
	}
}