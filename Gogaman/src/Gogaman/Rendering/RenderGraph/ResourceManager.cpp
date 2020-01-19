#include "pch.h"
#include "ResourceManager.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		void ResourceManager::AddResource(const std::string &name, const Texture &texture, const Graph::StageIndex lifetimeBegin)
		{
			GM_ASSERT(!m_ResourceNameIndices.count(name), "Failed to add resource | Texture \"%s\" is already present", name.c_str());

			m_ResourceNameIndices.emplace(name, m_TextureResources.size());

			TextureResource &resource = m_TextureResources.emplace_back();
			resource.texture        = texture;
			resource.lifetime.begin = lifetimeBegin;
			resource.lifetime.end   = lifetimeBegin;
		}

		void ResourceManager::AddResource(const std::string &name, const Buffer &buffer, const Graph::StageIndex lifetimeBegin)
		{
			GM_ASSERT(!m_ResourceNameIndices.count(name), "Failed to add resource | Buffer \"%s\" is already present", name.c_str());

			m_ResourceNameIndices.emplace(name, m_BufferResources.size());

			BufferResource &resource = m_BufferResources.emplace_back();
			resource.buffer         = buffer;
			resource.lifetime.begin = lifetimeBegin;
			resource.lifetime.end   = lifetimeBegin;
		}

		void ResourceManager::ExtendTextureLifetime(const std::string &name, const Graph::StageIndex lifetimeEnd)
		{
			GM_ASSERT(m_ResourceNameIndices.count(name), "Failed to extend texture lifetime | Invalid name");

			TextureResource &resource = m_TextureResources[m_ResourceNameIndices[name]];
			resource.lifetime.end = std::max(resource.lifetime.end, lifetimeEnd);
		}

		void ResourceManager::ExtendBufferLifetime(const std::string &name, const Graph::StageIndex lifetimeEnd)
		{
			GM_ASSERT(m_ResourceNameIndices.count(name), "Failed to extend buffer lifetime | Invalid name");

			BufferResource &resource = m_BufferResources[m_ResourceNameIndices[name]];
			resource.lifetime.end = std::max(resource.lifetime.end, lifetimeEnd);
		}

		void ResourceManager::AddAlias(const std::string &name, const std::string &alias)
		{
			GM_DEBUG_ASSERT(m_ResourceNameIndices.count(name), "Failed to add alias | Invalid name");
			GM_DEBUG_ASSERT(!alias.empty(),                    "Failed to add alias | Invalid alias");

			const auto index             = m_ResourceNameIndices[name];
			m_ResourceNameIndices[alias] = index;

			//OR bind flags together

			//Extend lifetime
		}

		void ResourceManager::Initialize()
		{
			for(auto &i : m_TextureResources)
			{
				i.identifier = g_Device->GetResources().textures.Create(i.texture.format, i.texture.width, i.texture.height, i.texture.depth, i.texture.levelCount);
			}

			for(auto &i : m_BufferResources)
			{
				i.identifier = g_Device->GetResources().buffers.Create(i.buffer.memoryType, i.buffer.size, i.buffer.bindFlags);
			}
		}

		/*
		void ResourceManager::CreateTexture(const std::string &name)
		{
			GM_DEBUG_ASSERT(m_ResourceNameIndices.count(name), "Failed to allocate texture | Invalid name");

			TextureResource &resource = m_TextureResources[m_ResourceNameIndices[name]];
			resource.identifier = g_Device->GetResources().textures.Create(resource.texture.format, resource.texture.width, resource.texture.height, resource.texture.depth, resource.texture.levelCount);
		}

		void ResourceManager::DestroyTexture(const std::string &name)
		{
			GM_DEBUG_ASSERT(m_ResourceNameIndices.count(name), "Failed to destroy texture | Name \"%s\" is invalid", name.c_str());

			g_Device->GetResources().textures.Destroy(m_TextureResources[m_ResourceNameIndices[name]].identifier);
		}
		*/
		void ResourceManager::AllocateTexture(const std::string &name)
		{
			GM_DEBUG_ASSERT(m_ResourceNameIndices.count(name), "Failed to allocate texture | Name \"%s\" is invalid", name.c_str());

			//TextureResource &resource = m_TextureResources[m_ResourceNameIndices[name]];
		}

		void ResourceManager::DeallocateTexture(const std::string &name)
		{
			GM_DEBUG_ASSERT(m_ResourceNameIndices.count(name), "Failed to deallocate texture | Name \"%s\" is invalid", name.c_str());

			g_Device->GetResources().textures.Destroy(m_TextureResources[m_ResourceNameIndices[name]].identifier);
		}
		/*
		void ResourceManager::CreateBuffer(const std::string &name)
		{
			GM_DEBUG_ASSERT(m_ResourceNameIndices.count(name), "Failed to create buffer | Name \"%s\" is invalid", name.c_str());

			BufferResource &resource = m_BufferResources[m_ResourceNameIndices[name]];
			resource.identifier = g_Device->GetResources().buffers.Create(resource.buffer.memoryType, resource.buffer.size, resource.buffer.bindFlags);
		}

		void ResourceManager::DestroyBuffer(const std::string &name)
		{
			GM_DEBUG_ASSERT(m_ResourceNameIndices.count(name), "Failed to destroy buffer | Name \"%s\" is invalid", name.c_str());

			g_Device->GetResources().buffers.Destroy(m_BufferResources[m_ResourceNameIndices[name]].identifier);
		}*/

		void ResourceManager::AllocateBuffer(const std::string &name)
		{
			GM_DEBUG_ASSERT(m_ResourceNameIndices.count(name), "Failed to allocate buffer | Name \"%s\" is invalid", name.c_str());

			//BufferResource &resource = m_BufferResources[m_ResourceNameIndices[name]];
		}

		void ResourceManager::DeallocateBuffer(const std::string &name)
		{
			GM_DEBUG_ASSERT(m_ResourceNameIndices.count(name), "Failed to deallocate buffer | Name \"%s\" is invalid", name.c_str());

			//BufferResource &resource = m_BufferResources[m_ResourceNameIndices[name]];
		}

		RHI::TextureID ResourceManager::GetTextureID(const std::string &name)
		{
			GM_DEBUG_ASSERT(m_ResourceNameIndices.count(name), "Failed to get texture ID | Name \"%s\" is invalid", name.c_str());

			return m_TextureResources[m_ResourceNameIndices[name]].identifier;
		}

		RHI::Texture &ResourceManager::GetTexture(const std::string &name)
		{
			GM_DEBUG_ASSERT(m_ResourceNameIndices.count(name), "Failed to get texture | Name \"%s\" is invalid", name.c_str());

			return g_Device->GetResources().textures.Get(GetTextureID(name));
		}

		RHI::BufferID ResourceManager::GetBufferID(const std::string &name)
		{
			GM_DEBUG_ASSERT(m_ResourceNameIndices.count(name), "Failed to get buffer ID | Name \"%s\" is invalid", name.c_str());

			return m_BufferResources[m_ResourceNameIndices[name]].identifier;
		}

		RHI::Buffer &ResourceManager::GetBuffer(const std::string &name)
		{
			GM_DEBUG_ASSERT(m_ResourceNameIndices.count(name), "Failed to get buffer | Name \"%s\" is invalid", name.c_str());

			return g_Device->GetResources().buffers.Get(GetBufferID(name));
		}
	}
}