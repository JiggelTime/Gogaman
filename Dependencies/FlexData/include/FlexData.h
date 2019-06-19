#pragma once
#pragma warning(disable:4996)

#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <functional>
#include <vector>

#define FLEX_HEADER_DATA_SIGNATURE 70, 108, 101, 120
#define FLEX_HEADER_DATA_VERSION   1
#define FLEX_VERTEX_DATA_SIZE      44

namespace FlexData
{
	struct FlexHeaderData
	{
		uint8_t signature[4] = { 70, 108, 101, 120 };
		uint8_t version      = 1;
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
		std::vector<FlexVertexData> vertexBufferData;
		std::vector<uint16_t>       indexBufferData;
	};

	enum class FlexTextureFormat : uint8_t
	{
		None = 0,
		R8, R16, R16F, R32, R32F,
		RGBA8, RGBA8SRGB,
		RGBA16, RGBA16F
	};

	struct FlexTextureData
	{
		uint16_t           width;
		uint16_t           height;
		FlexTextureFormat  format;
		uint8_t           *data;
	};

	struct FlexMaterialData
	{
		std::vector<FlexTextureData> textures;
	};

	struct FlexDataFormat
	{
		//Header
		uint8_t signature[4] = { FLEX_HEADER_DATA_SIGNATURE };
		uint8_t version      = FLEX_HEADER_DATA_VERSION;
		//Meshes
		uint16_t  numMeshes;
		uint32_t  meshDataSize;
		uint8_t  *meshData;
	};
	
	struct FlexData
	{
		FlexHeaderData            header;
		std::vector<FlexMeshData> meshes;
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

			//Mesh metadata
			//Read number of meshes
			uint8_t numMeshesData[sizeof(uint16_t)];
			fread(numMeshesData, 1, sizeof(uint16_t), file);
			uint16_t numMeshes = *(uint16_t *)numMeshesData;
			dataPayload.meshes.reserve(numMeshes);
			//Read mesh data size
			uint32_t meshDataSize;
			fread(&meshDataSize, sizeof(meshDataSize), 1, file);

			//Mesh data
			for(uint16_t i = 0; i < numMeshes; i++)
			{
				FlexMeshData meshDataPayload;
				//Read mesh vertex buffer data size
				uint32_t vertexBufferDataSize;
				fread(&vertexBufferDataSize, sizeof(uint32_t), 1, file);
				meshDataPayload.vertexBufferData.reserve(vertexBufferDataSize / FLEX_VERTEX_DATA_SIZE);
				//Read mesh vertex buffer data
				std::vector<FlexVertexData> vertexBufferDataTemp(vertexBufferDataSize / FLEX_VERTEX_DATA_SIZE);
				meshDataPayload.vertexBufferData = std::move(vertexBufferDataTemp);
				fread(&meshDataPayload.vertexBufferData[0], 1, vertexBufferDataSize, file);
				//Read mesh index buffer data size
				uint32_t indexBufferDataSize;
				fread(&indexBufferDataSize, sizeof(uint32_t), 1, file);
				meshDataPayload.indexBufferData.reserve(indexBufferDataSize / sizeof(uint16_t));
				//Read mesh index buffer data
				std::vector<uint16_t> indexBufferDataTemp(indexBufferDataSize / sizeof(uint16_t));
				meshDataPayload.indexBufferData = std::move(indexBufferDataTemp);
				fread(&meshDataPayload.indexBufferData[0], 1, indexBufferDataSize, file);

				dataPayload.meshes.emplace_back(std::move(meshDataPayload));
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
		FlexDataFormat dataPayload;
		if(data.meshes.size() > UINT16_MAX)
		{
			std::cerr << "Failed to export FlexData: number of meshes exceeds " << UINT16_MAX << std::endl;
			exit(1);
		}
		dataPayload.numMeshes = static_cast<uint16_t>(data.meshes.size());

		uint32_t meshDataSize = 0;
		meshDataSize += static_cast<uint32_t>(data.meshes.size() * sizeof(uint32_t) * 2);
		for(auto i : data.meshes)
			meshDataSize += static_cast<uint32_t>((i.vertexBufferData.size() * FLEX_VERTEX_DATA_SIZE) + (i.indexBufferData.size() * sizeof(uint16_t)));
		dataPayload.meshDataSize = meshDataSize;

		FILE *file = fopen(filepath, "wb");
		if(file)
		{
			//Header data
			//Write header signature
			fwrite(&dataPayload.signature, 1, sizeof(uint8_t) * 4, file);
			//Write header version
			fwrite(&dataPayload.version, sizeof(dataPayload.version), 1, file);

			//Mesh metadata
			//Write number of meshes
			fwrite(&dataPayload.numMeshes,    sizeof(dataPayload.numMeshes),    1, file);
			//Write mesh data size
			fwrite(&dataPayload.meshDataSize, sizeof(dataPayload.meshDataSize), 1, file);

			//Mesh data
			for(auto i : data.meshes)
			{
				if(i.vertexBufferData.size()  > UINT16_MAX)
				{
					std::cerr << "Failed to export FlexData mesh: number of vertices exceeds "  << UINT16_MAX << std::endl;
					exit(1);
				}

				uint32_t vertexBufferDataSize = i.vertexBufferData.size() * FLEX_VERTEX_DATA_SIZE;
				uint32_t indexBufferDataSize  = i.indexBufferData.size()  * sizeof(uint16_t);
				
				//Write vertex buffer data size
				fwrite(&vertexBufferDataSize, sizeof(vertexBufferDataSize), 1, file);
				//Write vertex buffer data
				fwrite(&i.vertexBufferData[0], 1, vertexBufferDataSize, file);
				//Write index buffer data size
				fwrite(&indexBufferDataSize, sizeof(indexBufferDataSize), 1, file);
				//Write index buffer data
				fwrite(&i.indexBufferData[0], 1, indexBufferDataSize, file);
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
}