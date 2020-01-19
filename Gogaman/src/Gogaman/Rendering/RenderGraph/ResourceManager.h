#pragma once

#include "Resource.h"

#include "Stage.h"

#include "Graph.h"

#include "Gogaman/RenderHardwareInterface/Identifier.h"
#include "Gogaman/RenderHardwareInterface/Texture.h"
#include "Gogaman/RenderHardwareInterface/Buffer.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		//TODO: Rename to ResourceRegistry
		class ResourceManager
		{
		private:
			using ResourceIndex = uint16_t;
			
			struct ResourceLifetime
			{
				Graph::StageIndex begin, end;
			};

			struct TextureResource
			{
				Texture          texture;
				ResourceLifetime lifetime;
				RHI::TextureID   identifier;
			};

			struct BufferResource
			{
				Buffer           buffer;
				ResourceLifetime lifetime;
				RHI::BufferID    identifier;
			};
		public:
			ResourceManager()  = default;
			~ResourceManager() = default;

			void AddResource(const std::string &name, const Texture &texture, const Graph::StageIndex lifetimeBegin);
			void AddResource(const std::string &name, const Buffer &buffer, const Graph::StageIndex lifetimeBegin);

			void ExtendTextureLifetime(const std::string &name, const Graph::StageIndex lifetimeEnd);
			void ExtendBufferLifetime(const std::string &name, const Graph::StageIndex lifetimeEnd);

			void AddAlias(const std::string &name, const std::string &alias);

			void Initialize();
			//void CreateTexture(const std::string &name);
			//void DestroyTexture(const std::string &name);
			void AllocateTexture(const std::string &name);
			void DeallocateTexture(const std::string &name);

			//void CreateBuffer(const std::string &name);
			//void DestroyBuffer(const std::string &name);
			void AllocateBuffer(const std::string &name);
			void DeallocateBuffer(const std::string &name);

			RHI::TextureID GetTextureID(const std::string &name);

			RHI::Texture &GetTexture(const std::string &name);

			RHI::BufferID GetBufferID(const std::string &name);

			RHI::Buffer &GetBuffer(const std::string &name);
		private:
			std::unordered_map<std::string, ResourceIndex> m_ResourceNameIndices;

			std::vector<TextureResource> m_TextureResources;
			std::vector<BufferResource>  m_BufferResources;
		};
	}
}