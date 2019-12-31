#pragma once

#include "FlexData.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class TextureImporter
{
public:
	static inline FlexData::FlexTextureData ImportAlbedo(const char *filepath)
	{
		FlexData::FlexTextureData textureData;
		if(Import(textureData, filepath, 1, 4))
			return textureData;
		else
			return FlexData::GetDefaultAlbedoTexture();
	}

	static inline FlexData::FlexTextureData ImportNormal(const char *filepath)
	{
		FlexData::FlexTextureData textureData;
		if(Import(textureData, filepath, 1, 4))
			return textureData;
		else
			return FlexData::GetDefaultNormalTexture();
	}

	static inline FlexData::FlexTextureData ImportRoughness(const char *filepath)
	{
		FlexData::FlexTextureData textureData;
		if(Import(textureData, filepath, 1, 1))
			return textureData;
		else
			return FlexData::GetDefaultRoughnessTexture();
	}

	static inline FlexData::FlexTextureData ImportMetalness(const char *filepath)
	{
		FlexData::FlexTextureData textureData;
		if(Import(textureData, filepath, 1, 1))
			return textureData;
		else
			return FlexData::GetDefaultMetalnessTexture();
	}

	static inline FlexData::FlexTextureData ImportEmissivity(const char *filepath)
	{
		FlexData::FlexTextureData textureData;
		if(Import(textureData, filepath, 1, 1))
			return textureData;
		else
			return FlexData::GetDefaultEmissivityTexture();
	}
private:
	TextureImporter()  = delete;
	~TextureImporter() = delete;

	static inline bool Import(FlexData::FlexTextureData &flexTextureData, const char *filepath, const uint8_t channelDataSize, const uint8_t channelCount)
			{
				int importWidth, importHeight, importChannels;
				stbi_set_flip_vertically_on_load(true);
				uint8_t *data = stbi_load(filepath, &importWidth, &importHeight, &importChannels, channelCount);
				if(!data)
				{
					std::cout << "Failed to import texture | Location: " << filepath << std::endl;
					return false;
				}

				if(importChannels != channelCount)
					std::cout << "Warning: texture has " << importChannels << " channels | Expected " << +channelCount << " | Location: " << filepath << std::endl;

				flexTextureData.width  = importWidth;
				flexTextureData.height = importHeight;

				const uint64_t dataSize = channelDataSize * channelCount * importWidth * importHeight;
				flexTextureData.data = new uint8_t[dataSize];
				memcpy(flexTextureData.data, data, dataSize);

				STBI_FREE(data);
				return true;
			}
};