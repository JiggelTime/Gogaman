#include "pch.h"
#include "StageBuilder.h"

#include "Gogaman/Core/Base.h"

#include "Resource.h"

#include "Stage.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		void StageBuilder::CreateTexture(const Texture &texture)
		{
			for(const Texture &i : m_Stage->m_CreateTextures)
			{
				GM_DEBUG_ASSERT(i.name != texture.name, "Failed to create texture | Texture \"%s\" is already present", texture.name);
			}

			m_Stage->m_CreateTextures.emplace_back(texture);
		}

		void StageBuilder::ReadTexture(const std::string &name)
		{
			for(const std::string &i : m_Stage->m_ReadTextureNames)
			{
				GM_DEBUG_ASSERT(i != name, "Failed to read texture | Texture \"%s\" is already present", name);
			}

			m_Stage->m_ReadTextureNames.emplace_back(name);
		}

		void StageBuilder::WriteTexture(const std::string &inputName, const std::string &outputName)
		{
			for(const std::string &i : m_Stage->m_WriteTextureInputNames)
			{
				GM_DEBUG_ASSERT(i != inputName, "Failed to write texture | Input texture \"%s\" is already present", inputName);
			}

			for(const std::string &i : m_Stage->m_WriteTextureOutputNames)
			{
				GM_DEBUG_ASSERT(i != outputName, "Failed to write texture | Output texture \"%s\" is already present", outputName);
			}

			m_Stage->m_WriteTextureInputNames.emplace_back(inputName);
			m_Stage->m_WriteTextureOutputNames.emplace_back(outputName);
		}

		void StageBuilder::CreateBuffer(const Buffer &buffer)
		{
			for(const Buffer &i : m_Stage->m_CreateBuffers)
			{
				GM_DEBUG_ASSERT(i.name != buffer.name, "Failed to create buffer | Buffer \"%s\" is already present", buffer.name);
			}

			m_Stage->m_CreateBuffers.emplace_back(buffer);
		}

		void StageBuilder::ReadBuffer(const std::string &name)
		{
			for(const std::string &i : m_Stage->m_ReadBufferNames)
			{
				GM_DEBUG_ASSERT(i != name, "Failed to read buffer | Buffer \"%s\" is already present", name);
			}

			m_Stage->m_ReadBufferNames.emplace_back(name);
		}

		void StageBuilder::WriteBuffer(const std::string &inputName, const std::string &outputName)
		{
			for(const std::string &i : m_Stage->m_WriteBufferInputNames)
			{
				GM_DEBUG_ASSERT(i != inputName, "Failed to write buffer | Input buffer \"%s\" is already present", inputName);
			}

			for(const std::string &i : m_Stage->m_WriteBufferOutputNames)
			{
				GM_DEBUG_ASSERT(i != outputName, "Failed to write buffer | Output buffer \"%s\" is already present", outputName);
			}

			m_Stage->m_WriteBufferInputNames.emplace_back(inputName);
			m_Stage->m_WriteBufferOutputNames.emplace_back(outputName);
		}
	}
}