#pragma once
#pragma warning(disable:4996)

#include <stdlib.h>
#include <iostream>
#include <functional>
#include <stdint.h>
#include <vector>

//"Flex" encoded in ASCII
#define FLEX_HEADER_DATA_SIGNATURE 70, 108, 101, 120
#define FLEX_HEADER_DATA_VERSION   9

//Unit: bytes
#define FLEX_HEADER_DATA_SIZE            (sizeof(uint8_t) * (4 + 1))
#define FLEX_MESH_TRANSFORM_DATA_SIZE    (sizeof(float) * (3 + 4 + 3))
#define FLEX_VERTEX_DATA_SIZE            (sizeof(float) * (3 + 2 + 3 + 3))
#define FLEX_BOUNDING_SPHERE_DATA_SIZE   (sizeof(float) * (3 + 1))
#define FLEX_BOUNDING_BOX_3D_DATA_SIZE   (sizeof(float) * (3 * 2))
#define FLEX_POINT_LIGHT_DATA_SIZE       (sizeof(float) * (3 + 3))
#define FLEX_DIRECTIONAL_LIGHT_DATA_SIZE (sizeof(float) * (3 + 3))

#define FLEX_DEFAULT_ALBEDO_TEXTURE_VALUE     128, 128, 128, 255
#define FLEX_DEFAULT_NORMAL_TEXTURE_VALUE     128, 128, 255, 255
#define FLEX_DEFAULT_ROUGHNESS_TEXTURE_VALUE  255
#define FLEX_DEFAULT_METALNESS_TEXTURE_VALUE  0
#define FLEX_DEFAULT_EMISSIVITY_TEXTURE_VALUE 0

namespace FlexData
{
	struct FlexHeaderData
	{
		const uint8_t signature[4] = { FLEX_HEADER_DATA_SIGNATURE };
		const uint8_t version      = FLEX_HEADER_DATA_VERSION;
	};

	struct FlexTextureData
	{
		//TODO: Supercompress data
		uint16_t  width;
		uint16_t  height;
		uint8_t   *data;
	};

	struct FlexTransformData
	{
		float position[3];
		float rotation[4];
		float scale[3];
	};

	struct FlexVertexData
	{
		bool operator==(const FlexVertexData &) const = default;

		float position[3];
		float uv[2];
		float normal[3];
		float tangent[3];
	};

	struct HashFlexVertexData
	{
		inline uint32_t operator()(const FlexVertexData &other) const
		{
			uint32_t digest = 0;

			const auto HashFloat = [&](const float value) { digest ^= std::hash<float>()(value) + 0x9e3779b9 + (digest << 6) + (digest >> 2); };

			HashFloat(other.position[0]);
			HashFloat(other.position[1]);
			HashFloat(other.position[2]);
			HashFloat(other.uv[0]);
			HashFloat(other.uv[1]);
			HashFloat(other.normal[0]);
			HashFloat(other.normal[1]);
			HashFloat(other.normal[2]);
			HashFloat(other.tangent[0]);
			HashFloat(other.tangent[1]);
			HashFloat(other.tangent[2]);

			return digest;
		}
	};

	struct FlexSphereData
	{
		float position[3];
		float radius;
	};

	struct FlexBoundingBox3D_Data
	{
		float minimum[3];
		float maximum[3];
	};

	struct FlexMeshData
	{
		//TODO: Support instances, each instance has its own transform (and maybe material index?)
		//std::vector<FlexTransformData> transforms;
		FlexTransformData           transform;
		//Buffers (shared among instances)
		std::vector<FlexVertexData> vertexBuffer;
		std::vector<uint16_t>       indexBuffer;
		//Bounding volumes
		FlexSphereData              boundingSphere;
		FlexBoundingBox3D_Data      axisAlignedBoundingBox;
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
	
	struct FlexData
	{
		FlexData &operator+=(const FlexData &other)
		{
			if(this->header.version != other.header.version)
			{
				std::cerr << "Failed to merge FlexData | Incompatible header version" << std::endl;
				return *this;
			}

			this->albedoTextures.insert(this->albedoTextures.end(), std::make_move_iterator(other.albedoTextures.begin()), std::make_move_iterator(other.albedoTextures.end()));
			this->normalTextures.insert(this->normalTextures.end(), std::make_move_iterator(other.normalTextures.begin()), std::make_move_iterator(other.normalTextures.end()));
			this->roughnessTextures.insert(this->roughnessTextures.end(), std::make_move_iterator(other.roughnessTextures.begin()), std::make_move_iterator(other.roughnessTextures.end()));
			this->metalnessTextures.insert(this->metalnessTextures.end(), std::make_move_iterator(other.metalnessTextures.begin()), std::make_move_iterator(other.metalnessTextures.end()));
			this->emissivityTextures.insert(this->emissivityTextures.end(), std::make_move_iterator(other.emissivityTextures.begin()), std::make_move_iterator(other.emissivityTextures.end()));

			this->meshes.insert(this->meshes.end(), std::make_move_iterator(other.meshes.begin()), std::make_move_iterator(other.meshes.end()));
			
			this->pointLights.insert(this->pointLights.end(), std::make_move_iterator(other.pointLights.begin()), std::make_move_iterator(other.pointLights.end()));
			
			this->directionalLights.insert(this->directionalLights.end(), std::make_move_iterator(other.directionalLights.begin()), std::make_move_iterator(other.directionalLights.end()));

			return *this;
		}

		FlexHeaderData                        header;
		std::vector<FlexTextureData>          albedoTextures;
		std::vector<FlexTextureData>          normalTextures;
		std::vector<FlexTextureData>          roughnessTextures;
		std::vector<FlexTextureData>          metalnessTextures;
		std::vector<FlexTextureData>          emissivityTextures;
		std::vector<FlexMeshData>             meshes;
		std::vector<FlexPointLightData>       pointLights;
		std::vector<FlexDirectionalLightData> directionalLights;
	};

	static inline bool Import(FlexData &flexData, const char *filepath)
	{
		FILE *file = fopen(filepath, "rb");
		if(!file)
		{
			std::cerr << "Failed to import FlexData | Failed to open file | Location: " << filepath << std::endl;
			return false;
		}

		std::cout << "Reading header..." << std::endl;

		//Header
		FlexHeaderData importHeaderData;
		fread(&importHeaderData, 1, FLEX_HEADER_DATA_SIZE, file);
		if(memcmp(importHeaderData.signature, flexData.header.signature, sizeof(importHeaderData.signature)))
		{
			std::cerr << "Failed to import FlexData | Invalid header signature | Location: " << filepath << std::endl;
			return false;
		}

		if(importHeaderData.version != flexData.header.version)
		{
			std::cerr << "Failed to import FlexData | Invalid header version | Location: " << filepath << std::endl;
			return false;
		}

		auto ReadTextures = [&file](std::vector<FlexTextureData> &textures, const uint8_t texelDataSize)
		{
			//Texture count
			uint16_t textureCount;
			fread(&textureCount, sizeof(textureCount), 1, file);
			//Textures
			textures.reserve(textureCount);
			for(uint16_t i = 0; i < textureCount; i++)
			{
				FlexTextureData &texture = textures.emplace_back();
				//Width
				fread(&texture.width, sizeof(texture.width), 1, file);
				//Height
				fread(&texture.height, sizeof(texture.height), 1, file);
				//Data
				const uint64_t dataSize = texelDataSize * texture.width * texture.height;
				texture.data = new uint8_t[dataSize];
				fread(texture.data, 1, dataSize, file);
			}
		};

		std::cout << "Reading textures..." << std::endl;

		//Albedo textures
		ReadTextures(flexData.albedoTextures, 1 * 4);
		//Normal textures
		ReadTextures(flexData.normalTextures, 1 * 4);
		//Roughness textures
		ReadTextures(flexData.roughnessTextures, 1);
		//Metalness textures
		ReadTextures(flexData.metalnessTextures, 1);
		//Emissivity textures
		ReadTextures(flexData.emissivityTextures, 1);

		std::cout << "Reading meshes..." << std::endl;

		//Mesh count
		uint32_t meshCount;
		fread(&meshCount, 1, sizeof(meshCount), file);
		//Meshes
		flexData.meshes.reserve(meshCount);
		for(uint32_t i = 0; i < meshCount; i++)
		{
			FlexMeshData &mesh = flexData.meshes.emplace_back();
			//Transform
			fread(&mesh.transform, FLEX_MESH_TRANSFORM_DATA_SIZE, 1, file);
			//Vertex buffer data size
			uint64_t vertexBufferDataSize;
			fread(&vertexBufferDataSize, sizeof(vertexBufferDataSize), 1, file);
			//Vertex buffer data
			mesh.vertexBuffer.resize(vertexBufferDataSize / FLEX_VERTEX_DATA_SIZE);
			fread(mesh.vertexBuffer.data(), vertexBufferDataSize, 1, file);
			//Index buffer data size
			uint64_t indexBufferDataSize;
			fread(&indexBufferDataSize, sizeof(indexBufferDataSize), 1, file);
			//Index buffer data
			mesh.indexBuffer.resize(indexBufferDataSize / sizeof(uint16_t));
			fread(mesh.indexBuffer.data(), indexBufferDataSize, 1, file);
			//Read bounding sphere data
			fread(&mesh.boundingSphere, FLEX_BOUNDING_SPHERE_DATA_SIZE, 1, file);
			//Read AABB data
			fread(&mesh.axisAlignedBoundingBox, FLEX_BOUNDING_BOX_3D_DATA_SIZE, 1, file);
		}

		std::cout << "Reading lights..." << std::endl;

		//Point light count
		uint32_t pointLightCount;
		fread(&pointLightCount, sizeof(uint32_t), 1, file);
		//Point light data
		flexData.pointLights.resize(pointLightCount);
		fread(flexData.pointLights.data(), FLEX_POINT_LIGHT_DATA_SIZE, pointLightCount, file);

		//Directional light count
		uint32_t directionalLightCount;
		fread(&directionalLightCount, sizeof(uint32_t), 1, file);
		//Directional light data
		flexData.directionalLights.resize(directionalLightCount);
		fread(flexData.directionalLights.data(), FLEX_DIRECTIONAL_LIGHT_DATA_SIZE, directionalLightCount, file);

		fclose(file);

		std::cout << "Successfully imported FlexData | Location: " << filepath << std::endl;
		return true;
	}

	static inline bool Export(const FlexData &flexData, const char *filepath)
	{
		if(flexData.meshes.size() > UINT16_MAX)
		{
			std::cerr << "Failed to export FlexData | Mesh count exceeds " << UINT16_MAX << std::endl;
			return false;
		}

		FILE *file = fopen(filepath, "wb");
		if(!file)
		{
			std::cerr << "Failed to export FlexData | Failed to create file | Location: " << filepath << std::endl;
			return false;
		}

		std::cout << "Writing header..." << std::endl;

		//Header signature
		const uint8_t headerSignature[] = { FLEX_HEADER_DATA_SIGNATURE };
		fwrite(headerSignature, 1, sizeof(headerSignature), file);
		//Header version
		const uint8_t headerVersion = FLEX_HEADER_DATA_VERSION;
		fwrite(&headerVersion, sizeof(headerVersion), 1, file);

		auto WriteTextures = [&file](const std::vector<FlexTextureData> &textures, const uint8_t texelDataSize)
		{
			//Texture count
			const uint16_t textureCount = (uint16_t)textures.size();
			fwrite(&textureCount, sizeof(textureCount), 1, file);
			//Textures
			for(const FlexTextureData &i : textures)
			{
				//Width
				fwrite(&i.width, sizeof(i.width), 1, file);
				//Height
				fwrite(&i.height, sizeof(i.height), 1, file);
				//Data
				fwrite(i.data, 1, texelDataSize * i.width * i.height, file);
			}
		};

		std::cout << "Writing textures..." << std::endl;

		//Albedo textures
		WriteTextures(flexData.albedoTextures, 1 * 4);
		//Normal textures
		WriteTextures(flexData.normalTextures, 1 * 4);
		//Roughness textures
		WriteTextures(flexData.roughnessTextures, 1);
		//Metalness textures
		WriteTextures(flexData.metalnessTextures, 1);
		//Emissivity textures
		WriteTextures(flexData.emissivityTextures, 1);

		std::cout << "Writing meshes..." << std::endl;

		//Mesh count
		const uint32_t meshCount = (uint32_t)flexData.meshes.size();
		fwrite(&meshCount, sizeof(meshCount), 1, file);
		//Meshes
		for(const auto &i : flexData.meshes)
		{
			if(i.vertexBuffer.size() > UINT16_MAX)
			{
				std::cerr << "Failed to export FlexData mesh | Vertex count exceeds "  << UINT16_MAX << std::endl;
				return false;
			}

			//Transform data
			fwrite(&i.transform, FLEX_MESH_TRANSFORM_DATA_SIZE, 1, file);
			//Vertex buffer data size
			const uint64_t vertexBufferDataSize = FLEX_VERTEX_DATA_SIZE * i.vertexBuffer.size();
			fwrite(&vertexBufferDataSize, sizeof(vertexBufferDataSize), 1, file);
			//Vertex buffer data
			fwrite(i.vertexBuffer.data(), vertexBufferDataSize, 1, file);
			//Index buffer data size
			const uint64_t indexBufferDataSize = sizeof(uint16_t) * i.indexBuffer.size();
			fwrite(&indexBufferDataSize, sizeof(indexBufferDataSize), 1, file);
			//Index buffer data
			fwrite(i.indexBuffer.data(), indexBufferDataSize, 1, file);
			//Bounding sphere data
			fwrite(&i.boundingSphere, FLEX_BOUNDING_SPHERE_DATA_SIZE, 1, file);
			//AABB data
			fwrite(&i.axisAlignedBoundingBox, FLEX_BOUNDING_BOX_3D_DATA_SIZE, 1, file);
		}

		std::cout << "Writing lights..." << std::endl;

		//Point light Count
		const uint32_t pointLightCount = (uint32_t)flexData.pointLights.size();
		fwrite(&pointLightCount, sizeof(pointLightCount), 1, file);
		//Point lights
		fwrite(flexData.pointLights.data(), FLEX_POINT_LIGHT_DATA_SIZE, pointLightCount, file);

		//Directional light Count
		const uint32_t directionalLightCount = (uint32_t)flexData.directionalLights.size();
		fwrite(&directionalLightCount, sizeof(directionalLightCount), 1, file);
		//Directional lights
		fwrite(flexData.directionalLights.data(), FLEX_DIRECTIONAL_LIGHT_DATA_SIZE, directionalLightCount, file);

		fclose(file);

		std::cout << "Successfully exported FlexData | Location: " << filepath << std::endl;
		return true;
	}

	static inline FlexTextureData &GetDefaultAlbedoTexture()
	{
		static FlexTextureData defaultAlbedoTexture;
		defaultAlbedoTexture.width  = 2;
		defaultAlbedoTexture.height = 2;
		defaultAlbedoTexture.data   = new uint8_t[4 * 2 * 2]{ FLEX_DEFAULT_ALBEDO_TEXTURE_VALUE };
		return defaultAlbedoTexture;
	}

	static inline FlexTextureData &GetDefaultNormalTexture()
	{
		static FlexTextureData defaultNormalTexture;
		defaultNormalTexture.width  = 2;
		defaultNormalTexture.height = 2;
		defaultNormalTexture.data   = new uint8_t[4 * 2 * 2]{ FLEX_DEFAULT_NORMAL_TEXTURE_VALUE };
		return defaultNormalTexture;
	}

	static inline FlexTextureData &GetDefaultRoughnessTexture()
	{
		static FlexTextureData defaultRoughnessTexture;
		defaultRoughnessTexture.width  = 2;
		defaultRoughnessTexture.height = 2;
		defaultRoughnessTexture.data   = new uint8_t[1 * 2 * 2]{ FLEX_DEFAULT_ROUGHNESS_TEXTURE_VALUE };
		return defaultRoughnessTexture;
	}

	static inline FlexTextureData &GetDefaultMetalnessTexture()
	{
		static FlexTextureData defaultMetalnessTexture;
		defaultMetalnessTexture.width  = 2;
		defaultMetalnessTexture.height = 2;
		defaultMetalnessTexture.data   = new uint8_t[1 * 2 * 2]{ FLEX_DEFAULT_METALNESS_TEXTURE_VALUE };
		return defaultMetalnessTexture;
	}

	static inline FlexTextureData &GetDefaultEmissivityTexture()
	{
		static FlexTextureData defaultEmissivityTexture;
		defaultEmissivityTexture.width  = 2;
		defaultEmissivityTexture.height = 2;
		defaultEmissivityTexture.data   = new uint8_t[1 * 2 * 2]{ FLEX_DEFAULT_EMISSIVITY_TEXTURE_VALUE };
		return defaultEmissivityTexture;
	}

	static inline void Print(const FlexData &data, const bool printVertexData = false)
	{
		std::cout << "Mesh count: " << data.meshes.size() << std::endl;
		for(const auto &i : data.meshes)
		{
			std::cout << std::endl;
			std::cout << "	Mesh:" << std::endl;
			std::cout << "		-Position:        " << i.transform.position[0] << ", " << i.transform.position[1] << ", " << i.transform.position[2] << std::endl;
			std::cout << "		-Rotation:        " << i.transform.rotation[0] << ", " << i.transform.rotation[1] << ", " << i.transform.rotation[2] << std::endl;
			std::cout << "		-Scale:           " << i.transform.scale[0]    << ", " << i.transform.scale[1]    << ", " << i.transform.scale[2]    << std::endl;
			std::cout << "		-Vertices:        " << i.vertexBuffer.size()   << std::endl;
			std::cout << "		-Indices:         " << i.indexBuffer.size()    << std::endl;
			std::cout << "	   -Bounding sphere:" << std::endl;
			std::cout << "		    -Position: " << i.boundingSphere.position[0] << ", " << i.boundingSphere.position[1] << ", " << i.boundingSphere.position[2] << std::endl;
			std::cout << "		    -Radius:   " << i.boundingSphere.radius      << std::endl;
			std::cout << "	   -AABB:" << std::endl;
			std::cout << "		    -Minimum: " << i.axisAlignedBoundingBox.minimum[0] << ", " << i.axisAlignedBoundingBox.minimum[1] << ", " << i.axisAlignedBoundingBox.minimum[2] << std::endl;
			std::cout << "		    -Maximum: " << i.axisAlignedBoundingBox.maximum[0] << ", " << i.axisAlignedBoundingBox.maximum[1] << ", " << i.axisAlignedBoundingBox.maximum[2] << std::endl;
			if(printVertexData)
			{
				for(auto &j : i.vertexBuffer)
				{
					std::cout << std::endl;
					std::cout << "    -Vertex:" << std::endl;
					std::cout << " 	    -Position: " << j.position[0] << ", " << j.position[1] << ", " << j.position[2] << std::endl;
					std::cout << " 	    -UV:       " << j.uv[0]       << ", " << j.uv[1]       << std::endl;
					std::cout << " 	    -Normal:   " << j.normal[0]   << ", " << j.normal[1]   << ", " << j.normal[2]   << std::endl;
					std::cout << " 	    -Tangent:  " << j.tangent[0]  << ", " << j.tangent[1]  << ", " << j.tangent[2]  << std::endl;
				}

				for(auto &j : i.indexBuffer)
					std::cout << "		-Index: " << j << std::endl;
			}
		}
		std::cout << std::endl;

		std::cout << "Point light count: " << data.pointLights.size() << std::endl;
		for(const auto &i : data.pointLights)
		{
			std::cout << std::endl;
			std::cout << "	Point light:" << std::endl;
			std::cout << "		-Position (XYZ): " << i.position[0] << ", " << i.position[1] << ", " << i.position[2] << std::endl;
			std::cout << "		-Radiance (XYZ): " << i.radiance[0] << ", " << i.radiance[1] << ", " << i.radiance[2] << std::endl;
		}
		std::cout << std::endl;

		std::cout << "Directional light count: " << data.directionalLights.size() << std::endl;
		for(const auto &i : data.directionalLights)
		{
			std::cout << std::endl;
			std::cout << "	Directional light:" << std::endl;
			std::cout << "		-Direction (XYZ): " << i.direction[0] << ", " << i.direction[1] << ", " << i.direction[2] << std::endl;
			std::cout << "		-Radiance  (XYZ): " << i.radiance[0]  << ", " << i.radiance[1]  << ", " << i.radiance[2]  << std::endl;
		}
		std::cout << std::endl;

		auto PrintTextures = [](const std::vector<FlexTextureData> &textures, const char *name)
		{
			std::cout << name << " texture count: " << textures.size() << std::endl;
			for(const auto &i : textures)
			{
				std::cout << std::endl;
				std::cout << "	" << name << " texture:" << std::endl;
				std::cout << "		-Width: "  << i.width  << std::endl;
				std::cout << "		-Height: " << i.height << std::endl;
			}
			std::cout << std::endl;
		};

		PrintTextures(data.albedoTextures,     "Albedo");
		PrintTextures(data.normalTextures,     "Normal");
		PrintTextures(data.roughnessTextures,  "Roughness");
		PrintTextures(data.metalnessTextures,  "Metalness");
		PrintTextures(data.emissivityTextures, "Emissivity");
	}
}