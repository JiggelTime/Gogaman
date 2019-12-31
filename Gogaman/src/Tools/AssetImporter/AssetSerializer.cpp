#pragma once

#include "FlexData.h"

#include "FBX_Importer.h"
#include "TextureImporter.h"

#include <filesystem>

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		std::cerr << "Failed to run asset serializer | Invalid parameters" << std::endl;
		//return EXIT_FAILURE;
	}

	//const char *directory = argv[1];
	//const char *exportFilename = argv[2];

	const char *directory = "D:/dev/TestAssetGroup";

	const char *exportFilename = "D:/dev/TestAssetGroup/TestData.flex";

	std::vector<std::string> geometryFBX_Filepaths, albedoTextureFilepaths, normalTextureFilepaths, roughnessTextureFilepaths, metalnessTextureFilepaths, emissivityTextureFilepaths;
	for(const auto &i : std::filesystem::directory_iterator(directory))
	{
		if(!i.is_regular_file())
			continue;

		const std::string filepath = i.path().string();

		if(i.path().extension() == ".fbx")
		{
			geometryFBX_Filepaths.emplace_back(filepath);
			continue;
		}

		const std::string prefix = i.path().filename().string().substr(0, 3);

		if(prefix == "ta_")
		{
			albedoTextureFilepaths.emplace_back(filepath);
			continue;
		}
		else if(prefix == "tn_")
		{
			normalTextureFilepaths.emplace_back(filepath);
			continue;
		}
		else if(prefix == "tr_")
		{
			roughnessTextureFilepaths.emplace_back(filepath);
			continue;
		}
		else if(prefix == "tm_")
		{
			metalnessTextureFilepaths.emplace_back(filepath);
			continue;
		}
		else if(prefix == "te_")
		{
			emissivityTextureFilepaths.emplace_back(filepath);
			continue;
		}
	}

	FlexData::FlexData flexData;

	//Import FBX files
	{
		Gogaman::Tools::FBX_Importer importer;
		for(const std::string &i : geometryFBX_Filepaths)
		{
			flexData += importer.Import(i.c_str());
		}
	}
	
	//Import albedo texture files
	flexData.albedoTextures.reserve(albedoTextureFilepaths.size());
	for(const std::string &i : albedoTextureFilepaths)
	{
		flexData.albedoTextures.emplace_back(TextureImporter::ImportAlbedo(i.c_str()));
	}

	//Import normal texture files
	flexData.normalTextures.reserve(normalTextureFilepaths.size());
	for(const std::string &i : normalTextureFilepaths)
	{
		flexData.normalTextures.emplace_back(TextureImporter::ImportNormal(i.c_str()));
	}

	//Import roughness texture files
	flexData.roughnessTextures.reserve(roughnessTextureFilepaths.size());
	for(const std::string &i : roughnessTextureFilepaths)
	{
		flexData.roughnessTextures.emplace_back(TextureImporter::ImportRoughness(i.c_str()));
	}

	//Import metalness texture files
	flexData.metalnessTextures.reserve(metalnessTextureFilepaths.size());
	for(const std::string &i : metalnessTextureFilepaths)
	{
		flexData.metalnessTextures.emplace_back(TextureImporter::ImportMetalness(i.c_str()));
	}

	//Import emissivity texture files
	flexData.emissivityTextures.reserve(emissivityTextureFilepaths.size());
	for(const std::string &i : emissivityTextureFilepaths)
	{
		flexData.emissivityTextures.emplace_back(TextureImporter::ImportEmissivity(i.c_str()));
	}

	FlexData::Print(flexData);

	FlexData::Export(flexData, exportFilename);

	return EXIT_SUCCESS;
}