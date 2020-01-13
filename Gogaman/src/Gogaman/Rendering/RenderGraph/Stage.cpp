#include "pch.h"
#include "Stage.h"

#include "Gogaman/Core/Base.h"

#include "Resource.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		void Stage::CreateTexture(const std::string &name, const Texture &texture, const RHI::Texture::State state)
		{
			for(const auto &i : m_CreateTextureData)
			{
				GM_DEBUG_ASSERT(i.name != name, "Failed to create texture | Texture \"%s\" is already present", name.c_str());
			}

			auto &createData = m_CreateTextureData.emplace_back();
			createData.name    = name;
			createData.texture = texture;
			createData.state   = state;
		}

		void Stage::ReadTexture(const std::string &name, const RHI::Texture::State state)
		{
			for(const auto &i : m_ReadTextureData)
			{
				GM_DEBUG_ASSERT(i.name != name, "Failed to read texture | Texture \"%s\" is already present", name.c_str());
			}

			auto &readData = m_ReadTextureData.emplace_back();
			readData.name  = name;
			readData.state = state;
		}

		void Stage::WriteTexture(const std::string &inputName, const std::string &outputName, const RHI::Texture::State state)
		{
			for(const auto &i : m_WriteTextureData)
			{
				GM_DEBUG_ASSERT(i.inputName != inputName,   "Failed to write texture | Input texture \"%s\" is already present",  inputName.c_str());
				GM_DEBUG_ASSERT(i.outputName != outputName, "Failed to write texture | Output texture \"%s\" is already present", outputName.c_str());
			}

			auto &writeData = m_WriteTextureData.emplace_back();
			writeData.inputName  = inputName;
			writeData.outputName = outputName;
			writeData.state      = state;
		}

		void Stage::CreateBuffer(const std::string &name, const Buffer &buffer)
		{
			for(const auto &i : m_CreateBufferData)
			{
				GM_DEBUG_ASSERT(i.name != name, "Failed to create buffer | Buffer \"%s\" is already present", name.c_str());
			}

			auto &createData = m_CreateBufferData.emplace_back();
			createData.name   = name;
			createData.buffer = buffer;
		}

		void Stage::ReadBuffer(const std::string &name)
		{
			for(const std::string &i : m_ReadBufferNames)
			{
				GM_DEBUG_ASSERT(i != name, "Failed to read buffer | Buffer \"%s\" is already present", name.c_str());
			}

			m_ReadBufferNames.emplace_back(name);
		}

		void Stage::WriteBuffer(const std::string &inputName, const std::string &outputName)
		{
			for(const std::string &i : m_WriteBufferInputNames)
			{
				GM_DEBUG_ASSERT(i != inputName, "Failed to write buffer | Input buffer \"%s\" is already present", inputName.c_str());
			}

			for(const std::string &i : m_WriteBufferOutputNames)
			{
				GM_DEBUG_ASSERT(i != outputName, "Failed to write buffer | Output buffer \"%s\" is already present", outputName.c_str());
			}

			m_WriteBufferInputNames.emplace_back(inputName);
			m_WriteBufferOutputNames.emplace_back(outputName);
		}
	}
}