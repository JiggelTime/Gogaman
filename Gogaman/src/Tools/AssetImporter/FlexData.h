#pragma once
#pragma warning(disable:4996)

#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <functional>
#include <vector>

#define FLEX_HEADER_DATA_SIGNATURE 70, 108, 101, 120
#define FLEX_HEADER_DATA_VERSION   3

#define FLEX_MESH_TRANSFORM_DATA_SIZE    (sizeof(float) * (3 + 4 + 3))
#define FLEX_VERTEX_DATA_SIZE            (sizeof(float) * (3 + 2 + 3 + 3))
#define FLEX_POINT_LIGHT_DATA_SIZE       (sizeof(float) * (3 + 3))
#define FLEX_DIRECTIONAL_LIGHT_DATA_SIZE (sizeof(float) * (3 + 3))

namespace FlexData
{
	struct FlexHeaderData
	{
		uint8_t signature[4] = { FLEX_HEADER_DATA_SIGNATURE };
		uint8_t version      = FLEX_HEADER_DATA_VERSION;
	};

	struct FlexMeshTransformData
	{
		float position[3];
		float rotation[4];
		float scale[3];
	};

	struct FlexVertexData
	{
		bool operator==(const FlexVertexData &other) const
		{
			return !memcmp(this, &other, sizeof(*this));
		}

		float position[3];
		float uv[2];
		float normal[3];
		float tangent[3];
	};

	struct FlexVertexDataHashFunction
	{
		uint32_t operator()(const FlexVertexData &other) const
		{
			uint32_t digest = 0;
			std::hash<float> floatHashFunction;
			digest ^= floatHashFunction(other.position[0]) + 0x9e3779b9 + (digest << 6) + (digest >> 2);
			digest ^= floatHashFunction(other.position[1]) + 0x9e3779b9 + (digest << 6) + (digest >> 2);
			digest ^= floatHashFunction(other.position[2]) + 0x9e3779b9 + (digest << 6) + (digest >> 2);
			digest ^= floatHashFunction(other.uv[0])       + 0x9e3779b9 + (digest << 6) + (digest >> 2);
			digest ^= floatHashFunction(other.uv[1])       + 0x9e3779b9 + (digest << 6) + (digest >> 2);
			digest ^= floatHashFunction(other.normal[0])   + 0x9e3779b9 + (digest << 6) + (digest >> 2);
			digest ^= floatHashFunction(other.normal[1])   + 0x9e3779b9 + (digest << 6) + (digest >> 2);
			digest ^= floatHashFunction(other.normal[2])   + 0x9e3779b9 + (digest << 6) + (digest >> 2);
			digest ^= floatHashFunction(other.tangent[0])  + 0x9e3779b9 + (digest << 6) + (digest >> 2);
			digest ^= floatHashFunction(other.tangent[1])  + 0x9e3779b9 + (digest << 6) + (digest >> 2);
			digest ^= floatHashFunction(other.tangent[2])  + 0x9e3779b9 + (digest << 6) + (digest >> 2);
			return digest;
		}
	};

	struct FlexMeshData
	{
		FlexMeshTransformData       transform;
		//TODO: rename this to vertexBuffer
		std::vector<FlexVertexData> vertexBufferData;
		std::vector<uint16_t>       indexBufferData;
	};

	struct FlexPointLightData
	{
		float position[3];
		float radiance[3];
	};

	struct FlexDirectionalLightData
	{
		float direction[3];
		float radiance[3];
	};

	struct FlexTextureData
	{
		uint16_t  width;
		uint16_t  height;
		uint8_t   bytesPerPixel;
		uint8_t  *data;
	};

	struct FlexMaterialData
	{
		//XYZW8
		FlexTextureData albedo;
		//XYZW8
		FlexTextureData normal;
		//X8
		FlexTextureData roughness;
		//X8
		FlexTextureData metalness;
		//X8
		FlexTextureData emissivity;
	};

	struct FlexDataFormat
	{
		//Header
		uint8_t signature[4] = { FLEX_HEADER_DATA_SIGNATURE };
		uint8_t version      = FLEX_HEADER_DATA_VERSION;
		//Meshes
		uint32_t  numMeshes;
		uint8_t  *meshData;
		//Lights
		uint32_t  numPointLights;
		uint8_t  *pointLightData;
		uint32_t  numDirectionalLights;
		uint8_t  *directionalLightData;
		//Materials
		uint32_t  numMaterials;
		uint8_t  *materialData;
	};
	
	struct FlexData
	{
		FlexHeaderData                        header;
		std::vector<FlexMeshData>             meshes;
		std::vector<FlexPointLightData>       pointLights;
		std::vector<FlexDirectionalLightData> directionalLights;
		std::vector<FlexMaterialData>         materials;
	};

	static FlexData ImportFlexData(const char *filepath)
	{
		FlexData dataPayload;

		FILE *file = fopen(filepath, "rb");
		if(file)
		{
			//Header data
			//Read header signature
			uint8_t headerSignatureBuffer[sizeof(uint8_t) * 4];
			fread(headerSignatureBuffer, 1, sizeof(uint8_t) * 4, file);
			if(memcmp(headerSignatureBuffer, dataPayload.header.signature, sizeof(uint8_t) * 4))
			{
				std::cerr << "Failed to import FlexData: invalid file signature" << std::endl;
				exit(1);
			}
			//Read header version
			uint8_t headerVersionBuffer[sizeof(uint8_t)];
			fread(headerVersionBuffer, 1, sizeof(uint8_t), file);
			uint8_t headerVersion = *(uint8_t *)headerVersionBuffer;
			if(headerVersion != dataPayload.header.version)
			{
				std::cerr << "Failed to import FlexData: file version " << +headerVersion << " is incompatible with FlexData version " << +dataPayload.header.version << std::endl;
				exit(1);
			}
			dataPayload.header.version = headerVersion;

			//Mesh data
			//TODO: Try: "using FlexDataNumMeshesType = uint16_t;"
			//Read number of meshes
			uint8_t numMeshesData[sizeof(uint32_t)];
			fread(numMeshesData, 1, sizeof(uint32_t), file);
			uint32_t numMeshes = *(uint32_t *)numMeshesData;
			dataPayload.meshes.reserve(numMeshes);
			for(uint32_t i = 0; i < numMeshes; i++)
			{
				FlexMeshData meshDataPayload;
				//Read transform data
				fread(&meshDataPayload.transform, FLEX_MESH_TRANSFORM_DATA_SIZE, 1, file);
				//Read vertex buffer data size
				uint32_t vertexBufferDataSize;
				fread(&vertexBufferDataSize, sizeof(uint32_t), 1, file);
				meshDataPayload.vertexBufferData.reserve(vertexBufferDataSize / FLEX_VERTEX_DATA_SIZE);
				//Read vertex buffer data
				std::vector<FlexVertexData> vertexBufferData(vertexBufferDataSize / FLEX_VERTEX_DATA_SIZE);
				meshDataPayload.vertexBufferData = std::move(vertexBufferData);
				fread(&meshDataPayload.vertexBufferData[0], 1, vertexBufferDataSize, file);
				//Read index buffer data size
				uint32_t indexBufferDataSize;
				fread(&indexBufferDataSize, sizeof(uint32_t), 1, file);
				meshDataPayload.indexBufferData.reserve(indexBufferDataSize / sizeof(uint16_t));
				//Read index buffer data
				std::vector<uint16_t> indexBufferData(indexBufferDataSize / sizeof(uint16_t));
				meshDataPayload.indexBufferData = std::move(indexBufferData);
				//TODO: Try using std::vector data()
				fread(&meshDataPayload.indexBufferData[0], 1, indexBufferDataSize, file);

				dataPayload.meshes.emplace_back(std::move(meshDataPayload));
			}

			//Point light data
			//Read number of point lights
			uint8_t numPointLightsData[sizeof(uint32_t)];
			fread(numPointLightsData, 1, sizeof(uint32_t), file);
			uint32_t numPointLights = *(uint32_t *)numPointLightsData;
			dataPayload.pointLights.reserve(numPointLights);
			//Read point light data
			std::vector<FlexPointLightData> pointLightData(numPointLights);
			dataPayload.pointLights = std::move(pointLightData);
			uint32_t pointLightDataSize = FLEX_POINT_LIGHT_DATA_SIZE * numPointLights;
			fread(&dataPayload.pointLights[0], 1, pointLightDataSize, file);

			//Directional light data
			//Read number of directional lights
			uint8_t numDirectionalLightsData[sizeof(uint32_t)];
			fread(numDirectionalLightsData, 1, sizeof(uint32_t), file);
			uint32_t numDirectionalLights = *(uint32_t*)numDirectionalLightsData;
			dataPayload.directionalLights.reserve(numDirectionalLights);
			//Read point light data
			std::vector<FlexDirectionalLightData> directionalLightData(numDirectionalLights);
			dataPayload.directionalLights = std::move(directionalLightData);
			uint32_t directionalLightDataSize = FLEX_POINT_LIGHT_DATA_SIZE * numDirectionalLights;
			fread(&dataPayload.directionalLights[0], 1, directionalLightDataSize, file);
			
			//Material data
			//Read number of materials
			uint8_t numMaterialsData[sizeof(uint32_t)];
			fread(numMaterialsData, 1, sizeof(uint32_t), file);
			uint32_t numMaterials = *(uint32_t *)numMaterialsData;
			dataPayload.materials.reserve(numMaterials);
			for(uint32_t i = 0; i < numMaterials; i++)
			{
				auto ReadTexture = [](FILE *file, FlexTextureData &texturePayload)
				{
					//Read width
					fread(&texturePayload.width, sizeof(texturePayload.width), 1, file);
					//Read height
					fread(&texturePayload.height, sizeof(texturePayload.height), 1, file);
					//Read bytes per pixel
					fread(&texturePayload.bytesPerPixel, sizeof(texturePayload.bytesPerPixel), 1, file);
					//Read texture data
					uint32_t  textureDataSize = texturePayload.width * texturePayload.height * texturePayload.bytesPerPixel;
					uint8_t  *textureData = new uint8_t[textureDataSize];
					fread(textureData, 1, textureDataSize, file);
					texturePayload.data = textureData;
				};

				FlexMaterialData materialDataPayload;
				ReadTexture(file, materialDataPayload.albedo);
				ReadTexture(file, materialDataPayload.normal);
				ReadTexture(file, materialDataPayload.roughness);
				ReadTexture(file, materialDataPayload.metalness);
				ReadTexture(file, materialDataPayload.emissivity);

				dataPayload.materials.emplace_back(std::move(materialDataPayload));
			}

			fclose(file);
			std::cout << "Successfully imported FlexData | Location: " << filepath << std::endl;
		}
		else
		{
			std::cerr << "Failed to open file | Location: " << filepath << std::endl;
			exit(1);
		}

		return dataPayload;
	}

	static void ExportFlexData(const char *filepath, const FlexData &data)
	{
		if(data.meshes.size() > UINT16_MAX)
		{
			std::cerr << "Failed to export FlexData: number of meshes exceeds " << UINT16_MAX << std::endl;
			exit(1);
		}

		FILE *file = fopen(filepath, "wb");
		if(file)
		{
			FlexDataFormat dataPayload;

			//Header data
			//Write header signature
			fwrite(&dataPayload.signature, 1, sizeof(uint8_t) * 4, file);
			//Write header version
			fwrite(&dataPayload.version, sizeof(dataPayload.version), 1, file);

			//Mesh data
			//Write number of meshes
			dataPayload.numMeshes = data.meshes.size();
			fwrite(&dataPayload.numMeshes, sizeof(dataPayload.numMeshes), 1, file);
			for(const auto &i : data.meshes)
			{
				if(i.vertexBufferData.size()  > UINT16_MAX)
				{
					std::cerr << "Failed to export FlexData mesh: number of vertices exceeds "  << UINT16_MAX << std::endl;
					exit(1);
				}

				//Write transform data
				fwrite(&i.transform, FLEX_MESH_TRANSFORM_DATA_SIZE, 1, file);
				//Write vertex buffer data size
				uint32_t vertexBufferDataSize = FLEX_VERTEX_DATA_SIZE * i.vertexBufferData.size();
				fwrite(&vertexBufferDataSize, sizeof(vertexBufferDataSize), 1, file);
				//Write vertex buffer data
				//TODO: try using std::vector data()
				fwrite(&i.vertexBufferData[0], 1, vertexBufferDataSize, file);
				//Write index buffer data size
				uint32_t indexBufferDataSize = sizeof(uint16_t) * i.indexBufferData.size();
				fwrite(&indexBufferDataSize, sizeof(indexBufferDataSize), 1, file);
				//Write index buffer data
				fwrite(&i.indexBufferData[0], 1, indexBufferDataSize, file);
			}

			//Point light data
			//Write number of point lights
			dataPayload.numPointLights = data.pointLights.size();
			fwrite(&dataPayload.numPointLights, sizeof(dataPayload.numPointLights), 1, file);
			//Write point light data
			uint32_t pointLightDataSize = FLEX_POINT_LIGHT_DATA_SIZE * data.pointLights.size();
			fwrite(&data.pointLights[0], 1, pointLightDataSize, file);

			//Directional light data
			//Write number of directional lights
			dataPayload.numDirectionalLights = data.directionalLights.size();
			fwrite(&dataPayload.numDirectionalLights, sizeof(dataPayload.numDirectionalLights), 1, file);
			//Write directional light data
			uint32_t directionalLightDataSize = FLEX_DIRECTIONAL_LIGHT_DATA_SIZE * data.directionalLights.size();
			fwrite(&data.directionalLights[0], 1, directionalLightDataSize, file);

			//Material data
			//Write number of materials
			dataPayload.numMaterials = data.materials.size();
			fwrite(&dataPayload.numMaterials, sizeof(dataPayload.numMaterials), 1, file);
			for(const auto &i : data.materials)
			{
				auto WriteTexture = [](const FlexTextureData &texture, FILE *file)
				{
					//Write width
					fwrite(&texture.width,         sizeof(texture.width),         1, file);
					//Write height
					fwrite(&texture.height,        sizeof(texture.height),        1, file);
					//Write bytes per pixel
					fwrite(&texture.bytesPerPixel, sizeof(texture.bytesPerPixel), 1, file);
					//Write texture data
					fwrite(texture.data, 1, texture.width * texture.height * texture.bytesPerPixel, file);
				};

				//Write textures
				WriteTexture(i.albedo,     file);
				WriteTexture(i.normal,     file);
				WriteTexture(i.roughness,  file);
				WriteTexture(i.metalness,  file);
				WriteTexture(i.emissivity, file);
			}

			fclose(file);
			std::cout << "Successfully exported FlexData | Location: " << filepath << std::endl;
		}
		else
		{
			std::cerr << "Failed to open file | Location: " << filepath << std::endl;
			exit(1);
		}
	}

	static void PrintFlexData(const FlexData &data)
	{
		std::cout << "[FlexData] Meshes: " << data.meshes.size() << std::endl;
		for(const auto &i : data.meshes)
		{
			std::cout << std::endl;
			std::cout << "[FlexData] Mesh:" << std::endl;
			std::cout << "[FlexData]	-Position (XYZ): " << i.transform.position[0] << ", " << i.transform.position[1] << ", " << i.transform.position[2] << std::endl;
			std::cout << "[FlexData]	-Rotation (XYZ): " << i.transform.rotation[0] << ", " << i.transform.rotation[1] << ", " << i.transform.rotation[2] << std::endl;
			std::cout << "[FlexData]	-Scale    (XYZ): " << i.transform.scale[0]    << ", " << i.transform.scale[1]    << ", " << i.transform.scale[2]    << std::endl;
			std::cout << "[FlexData]	-Vertices: " << i.vertexBufferData.size() << std::endl;
			std::cout << "[FlexData]	-Indices:  " << i.indexBufferData.size()  << std::endl;
		}
		std::cout << std::endl;

		std::cout << "[FlexData] Point lights: " << data.pointLights.size() << std::endl;
		for(const auto &i : data.pointLights)
		{
			std::cout << std::endl;
			std::cout << "[FlexData] Point light:" << std::endl;
			std::cout << "[FlexData]	-Position (XYZ): " << i.position[0] << ", " << i.position[1] << ", " << i.position[2] << std::endl;
			std::cout << "[FlexData]	-Radiance (XYZ): " << i.radiance[0] << ", " << i.radiance[1] << ", " << i.radiance[2] << std::endl;
		}
		std::cout << std::endl;

		std::cout << "[FlexData] Directional lights: " << data.directionalLights.size() << std::endl;
		for(const auto &i : data.directionalLights)
		{
			std::cout << std::endl;
			std::cout << "[FlexData] Directional light:" << std::endl;
			std::cout << "[FlexData]	-Direction (XYZ): " << i.direction[0] << ", " << i.direction[1] << ", " << i.direction[2] << std::endl;
			std::cout << "[FlexData]	-Radiance  (XYZ): " << i.radiance[0]  << ", " << i.radiance[1]  << ", " << i.radiance[2]  << std::endl;
		}
		std::cout << std::endl;

		std::cout << "[FlexData] Materials: " << data.materials.size() << std::endl;
		for(const auto &i : data.materials)
		{
			std::cout << std::endl;
			std::cout << "[FlexData] Material:" << std::endl;
			std::cout << "[FlexData]	-Albedo texture     | Width: " << i.albedo.width     << " | Height: " << i.albedo.height     << " | BPP: " << +i.albedo.bytesPerPixel     << " | First pixel (XYZW): " << +i.albedo.data[0]     << ", " << +i.albedo.data[1] << ", " << +i.albedo.data[2] << ", " << +i.albedo.data[3] << std::endl;
			std::cout << "[FlexData]	-Normal texture     | Width: " << i.normal.width     << " | Height: " << i.normal.height     << " | BPP: " << +i.normal.bytesPerPixel     << " | First pixel (XYZW): " << +i.normal.data[0]     << ", " << +i.normal.data[1] << ", " << +i.normal.data[2] << ", " << +i.normal.data[3] << std::endl;
			std::cout << "[FlexData]	-Roughness texture  | Width: " << i.roughness.width  << " | Height: " << i.roughness.height  << " | BPP: " << +i.roughness.bytesPerPixel  << " | First pixel (X): "    << +i.roughness.data[0]  << std::endl;
			std::cout << "[FlexData]	-Metalness texture  | Width: " << i.metalness.width  << " | Height: " << i.metalness.height  << " | BPP: " << +i.metalness.bytesPerPixel  << " | First pixel (X): "    << +i.metalness.data[0]  << std::endl;
			std::cout << "[FlexData]	-Emissivity texture | Width: " << i.emissivity.width << " | Height: " << i.emissivity.height << " | BPP: " << +i.emissivity.bytesPerPixel << " | First pixel (X): "    << +i.emissivity.data[0] << std::endl;
		}
		std::cout << std::endl;
	}
}