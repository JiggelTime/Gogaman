#pragma once
#pragma warning(disable:4996)

#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <functional>
#include <vector>

//"Flex" encoded in ASCII
#define FLEX_HEADER_DATA_SIGNATURE 70, 108, 101, 120
#define FLEX_HEADER_DATA_VERSION   7

//Unit: bytes
#define FLEX_MESH_TRANSFORM_DATA_SIZE    (sizeof(float) * (3 + 4 + 3))
#define FLEX_VERTEX_DATA_SIZE            (sizeof(float) * (3 + 2 + 3 + 3))
#define FLEX_BOUNDING_SPHERE_DATA_SIZE   (sizeof(float) * (3 + 1))
#define FLEX_BOUNDING_BOX_3D_DATA_SIZE   (sizeof(float) * (3 * 2))
#define FLEX_POINT_LIGHT_DATA_SIZE       (sizeof(float) * (3 + 3))
#define FLEX_DIRECTIONAL_LIGHT_DATA_SIZE (sizeof(float) * (3 + 3))

#define FLEX_DEFAULT_ALBEDO_TEXTURE_VALUE     128, 128, 128, 255
#define FLEX_DEFAULT_NORMAL_TEXTURE_VALUE     128, 128, 255, 255
#define FLEX_DEFAULT_ROUGHNESS_TEXTURE_VALUE  128
#define FLEX_DEFAULT_METALNESS_TEXTURE_VALUE  0
#define FLEX_DEFAULT_EMISSIVITY_TEXTURE_VALUE 0

namespace FlexData
{
	struct FlexHeaderData
	{
		uint8_t signature[4] = { FLEX_HEADER_DATA_SIGNATURE };
		uint8_t version      = FLEX_HEADER_DATA_VERSION;
	};

	struct FlexTransformData
	{
		float position[3];
		float rotation[4];
		float scale[3];
	};

	struct FlexVertexData
	{
		inline bool operator==(const FlexVertexData &other) const { return !memcmp(this, &other, sizeof(*this)); }

		float position[3];
		float uv[2];
		float normal[3];
		float tangent[3];
	};

	struct FlexVertexDataHashFunction
	{
		inline uint32_t operator()(const FlexVertexData &other) const
		{
			auto HashFloat = [](uint32_t &digest, const float value) { digest ^= std::hash<float>()(value) + 0x9e3779b9 + (digest << 6) + (digest >> 2); };
			
			uint32_t digest = 0;
			HashFloat(digest, other.position[0]);
			HashFloat(digest, other.position[1]);
			HashFloat(digest, other.position[2]);
			HashFloat(digest, other.uv[0]);
			HashFloat(digest, other.uv[1]);
			HashFloat(digest, other.normal[0]);
			HashFloat(digest, other.normal[1]);
			HashFloat(digest, other.normal[2]);
			HashFloat(digest, other.tangent[0]);
			HashFloat(digest, other.tangent[1]);
			HashFloat(digest, other.tangent[2]);
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
		uint32_t                    materialIndex;
		//Bounding volume
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

	struct FlexTextureData
	{
		//TODO: Use S3 texture compression (DXT1 format)
		uint16_t  width;
		uint16_t  height;
		uint8_t  *data;
	};

	struct FlexMaterialData
	{
		inline bool operator==(const FlexMaterialData &other) const
		{
			auto CompareFlexTexture = [](const uint8_t bytesPerPixel, const FlexTextureData &texture, const FlexTextureData &texture2){
				if((texture.width != texture2.width) || (texture.height != texture2.height))
					return false;
				else
				{
					for(uint64_t i = 0; i < bytesPerPixel * texture.width * texture.height; i++)
					{
						if(texture.data[i] != texture2.data[i])
							return false;
					}

					return true;
				}					
			};

			return CompareFlexTexture(4, albedo, other.albedo) && CompareFlexTexture(4, normal, other.normal) && CompareFlexTexture(1, roughness, other.roughness) && CompareFlexTexture(1, metalness, other.metalness) && CompareFlexTexture(1, emissivity, other.emissivity);
		}

		//RGBW8
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

	static inline FlexData ImportFlexData(const char *filepath)
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
			uint8_t headerVersion;
			fread(&headerVersion, 1, sizeof(uint8_t), file);
			if(headerVersion != dataPayload.header.version)
			{
				std::cerr << "Failed to import FlexData: file version " << +headerVersion << " is incompatible with FlexData version " << +dataPayload.header.version << std::endl;
				exit(1);
			}
			dataPayload.header.version = headerVersion;

			//Mesh data
			//Read number of meshes
			using numMeshesType = uint32_t;
			uint8_t numMeshesData[sizeof(numMeshesType)];
			fread(numMeshesData, 1, sizeof(numMeshesType), file);
			numMeshesType numMeshes = *(numMeshesType *)numMeshesData;
			//numMeshesType numMeshes = *reinterpret_cast<numMeshesType *>(numMeshesData);
			dataPayload.meshes.reserve(numMeshes);
			for(numMeshesType i = 0; i < numMeshes; i++)
			{
				FlexMeshData meshDataPayload;
				//Read transform data
				fread(&meshDataPayload.transform, FLEX_MESH_TRANSFORM_DATA_SIZE, 1, file);
				//Read vertex buffer data size
				uint64_t vertexBufferDataSize;
				fread(&vertexBufferDataSize, sizeof(vertexBufferDataSize), 1, file);
				//Read vertex buffer data
				meshDataPayload.vertexBuffer = std::vector<FlexVertexData>(vertexBufferDataSize / FLEX_VERTEX_DATA_SIZE);
				//std::vector<FlexVertexData> vertexBufferData(vertexBufferDataSize / FLEX_VERTEX_DATA_SIZE);
				//meshDataPayload.vertexBuffer = std::move(vertexBufferData);
				fread(meshDataPayload.vertexBuffer.data(), 1, vertexBufferDataSize, file);
				//Read index buffer data size
				uint64_t indexBufferDataSize;
				fread(&indexBufferDataSize, sizeof(vertexBufferDataSize), 1, file);
				//Read index buffer data
				meshDataPayload.indexBuffer = std::vector<uint16_t>(indexBufferDataSize / sizeof(uint16_t));
				fread(meshDataPayload.indexBuffer.data(), 1, indexBufferDataSize, file);
				//Read material index data
				fread(&meshDataPayload.materialIndex, sizeof(uint32_t), 1, file);
				//Read bounding sphere data
				fread(&meshDataPayload.boundingSphere, FLEX_BOUNDING_SPHERE_DATA_SIZE, 1, file);
				//Read AABB data
				fread(&meshDataPayload.axisAlignedBoundingBox, FLEX_BOUNDING_BOX_3D_DATA_SIZE, 1, file);

				dataPayload.meshes.emplace_back(std::move(meshDataPayload));
			}

			//Point light data
			//Read number of point lights
			uint32_t numPointLights;
			fread(&numPointLights, 1, sizeof(uint32_t), file);
			dataPayload.pointLights.reserve(numPointLights);
			//Read point light data
			std::vector<FlexPointLightData> pointLightData(numPointLights);
			dataPayload.pointLights = std::move(pointLightData);
			fread(dataPayload.pointLights.data(), 1, FLEX_POINT_LIGHT_DATA_SIZE * numPointLights, file);

			//Directional light data
			//Read number of directional lights
			uint32_t numDirectionalLights;
			fread(&numDirectionalLights, 1, sizeof(uint32_t), file);
			dataPayload.directionalLights.reserve(numDirectionalLights);
			//Read point light data
			std::vector<FlexDirectionalLightData> directionalLightData(numDirectionalLights);
			dataPayload.directionalLights = std::move(directionalLightData);
			fread(dataPayload.directionalLights.data(), 1, FLEX_POINT_LIGHT_DATA_SIZE * numDirectionalLights, file);
			
			//Material data
			//Read number of materials
			uint32_t numMaterials;
			fread(&numMaterials, 1, sizeof(uint32_t), file);
			dataPayload.materials.reserve(numMaterials);
			//Read material data
			for(uint32_t i = 0; i < numMaterials; i++)
			{
				auto ReadTexture = [](FILE *file, FlexTextureData &texturePayload, const uint8_t bytesPerPixel)
				{
					//Read width
					fread(&texturePayload.width, sizeof(texturePayload.width), 1, file);
					//Read height
					fread(&texturePayload.height, sizeof(texturePayload.height), 1, file);
					//Read texture data
					uint64_t  textureDataSize = texturePayload.width * texturePayload.height * bytesPerPixel;
					uint8_t  *textureData = new uint8_t[textureDataSize];
					fread(textureData, 1, textureDataSize, file);
					texturePayload.data = textureData;
				};

				FlexMaterialData materialDataPayload;
				ReadTexture(file, materialDataPayload.albedo,     4);
				ReadTexture(file, materialDataPayload.normal,     4);
				ReadTexture(file, materialDataPayload.roughness,  1);
				ReadTexture(file, materialDataPayload.metalness,  1);
				ReadTexture(file, materialDataPayload.emissivity, 1);

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
				if(i.vertexBuffer.size() > UINT16_MAX)
				{
					std::cerr << "Failed to export FlexData mesh: number of vertices exceeds "  << UINT16_MAX << std::endl;
					exit(1);
				}

				//Write transform data
				fwrite(&i.transform, FLEX_MESH_TRANSFORM_DATA_SIZE, 1, file);
				//Write vertex buffer data size
				uint64_t vertexBufferDataSize = FLEX_VERTEX_DATA_SIZE * i.vertexBuffer.size();
				fwrite(&vertexBufferDataSize, sizeof(vertexBufferDataSize), 1, file);
				//Write vertex buffer data
				fwrite(i.vertexBuffer.data(), 1, vertexBufferDataSize, file);
				//Write index buffer data size
				uint64_t indexBufferDataSize = sizeof(uint16_t) * i.indexBuffer.size();
				fwrite(&indexBufferDataSize, sizeof(indexBufferDataSize), 1, file);
				//Write index buffer data
				fwrite(i.indexBuffer.data(), 1, indexBufferDataSize, file);
				//Write material index data
				fwrite(&i.materialIndex, sizeof(uint32_t), 1, file);
				//Write bounding sphere data
				fwrite(&i.boundingSphere, FLEX_BOUNDING_SPHERE_DATA_SIZE, 1, file);
				//Write AABB data
				fwrite(&i.axisAlignedBoundingBox, FLEX_BOUNDING_BOX_3D_DATA_SIZE, 1, file);
			}

			//Point light data
			//Write number of point lights
			dataPayload.numPointLights = data.pointLights.size();
			fwrite(&dataPayload.numPointLights, sizeof(dataPayload.numPointLights), 1, file);
			//Write point light data
			uint64_t pointLightDataSize = FLEX_POINT_LIGHT_DATA_SIZE * data.pointLights.size();
			fwrite(data.pointLights.data(), 1, pointLightDataSize, file);

			//Directional light data
			//Write number of directional lights
			dataPayload.numDirectionalLights = data.directionalLights.size();
			fwrite(&dataPayload.numDirectionalLights, sizeof(dataPayload.numDirectionalLights), 1, file);
			//Write directional light data
			uint64_t directionalLightDataSize = FLEX_DIRECTIONAL_LIGHT_DATA_SIZE * data.directionalLights.size();
			fwrite(data.directionalLights.data(), 1, directionalLightDataSize, file);

			//Material data
			//Write number of materials
			dataPayload.numMaterials = data.materials.size();
			fwrite(&dataPayload.numMaterials, sizeof(dataPayload.numMaterials), 1, file);
			for(const auto &i : data.materials)
			{
				auto WriteTexture = [](FILE *file, const FlexTextureData &texture, const uint8_t bytesPerPixel)
				{
					//Write width
					fwrite(&texture.width,  sizeof(texture.width),  1, file);
					//Write height
					fwrite(&texture.height, sizeof(texture.height), 1, file);
					//Write texture data
					fwrite(texture.data, 1, texture.width * texture.height * bytesPerPixel, file);
				};

				//Write textures
				WriteTexture(file, i.albedo,     4);
				WriteTexture(file, i.normal,     4);
				WriteTexture(file, i.roughness,  1);
				WriteTexture(file, i.metalness,  1);
				WriteTexture(file, i.emissivity, 1);
			}

			fclose(file);
			std::cout << "Successfully exported FlexData | Location: " << filepath << std::endl;
		}
		else
		{
			std::cerr << "Failed to create file | Location: " << filepath << std::endl;
			exit(1);
		}
	}

	static const FlexTextureData GetDefaultAlbedoTexture()
	{
		static FlexTextureData defaultAlbedoTexture;
		defaultAlbedoTexture.width = 1;
		defaultAlbedoTexture.height = 1;
		defaultAlbedoTexture.data = new uint8_t[4]{ FLEX_DEFAULT_ALBEDO_TEXTURE_VALUE };
		return defaultAlbedoTexture;
	}

	static const FlexTextureData GetDefaultNormalTexture()
	{
		static FlexTextureData defaultNormalTexture;
		defaultNormalTexture.width = 1;
		defaultNormalTexture.height = 1;
		defaultNormalTexture.data = new uint8_t[4]{ FLEX_DEFAULT_NORMAL_TEXTURE_VALUE };
		return defaultNormalTexture;
	}

	static const FlexTextureData GetDefaultRoughnessTexture()
	{
		static FlexTextureData defaultRoughnessTexture;
		defaultRoughnessTexture.width = 1;
		defaultRoughnessTexture.height = 1;
		defaultRoughnessTexture.data = new uint8_t(FLEX_DEFAULT_ROUGHNESS_TEXTURE_VALUE);
		return defaultRoughnessTexture;
	}

	static const FlexTextureData GetDefaultMetalnessTexture()
	{
		static FlexTextureData defaultMetalnessTexture;
		defaultMetalnessTexture.width = 1;
		defaultMetalnessTexture.height = 1;
		defaultMetalnessTexture.data = new uint8_t(FLEX_DEFAULT_METALNESS_TEXTURE_VALUE);
		return defaultMetalnessTexture;
	}

	static const FlexTextureData GetDefaultEmissivityTexture()
	{
		static FlexTextureData defaultEmissivityTexture;
		defaultEmissivityTexture.width = 1;
		defaultEmissivityTexture.height = 1;
		defaultEmissivityTexture.data = new uint8_t(FLEX_DEFAULT_EMISSIVITY_TEXTURE_VALUE);
		return defaultEmissivityTexture;
	}

	static const FlexMaterialData GetDefaultMaterial()
	{
		static FlexMaterialData defaultMaterial;
		defaultMaterial.albedo = GetDefaultAlbedoTexture();
		defaultMaterial.normal = GetDefaultNormalTexture();
		defaultMaterial.roughness = GetDefaultRoughnessTexture();
		defaultMaterial.metalness = GetDefaultMetalnessTexture();
		defaultMaterial.emissivity = GetDefaultEmissivityTexture();
		return defaultMaterial;
	}

	static inline void PrintFlexData(const FlexData &data, const bool printVertexData = false)
	{
		std::cout << "[FlexData] Meshes: " << data.meshes.size() << std::endl;
		for(const auto &i : data.meshes)
		{
			std::cout << std::endl;
			std::cout << "[FlexData] Mesh:" << std::endl;
			std::cout << "[FlexData]	-Position:        " << i.transform.position[0] << ", " << i.transform.position[1] << ", " << i.transform.position[2] << std::endl;
			std::cout << "[FlexData]	-Rotation:        " << i.transform.rotation[0] << ", " << i.transform.rotation[1] << ", " << i.transform.rotation[2] << std::endl;
			std::cout << "[FlexData]	-Scale:           " << i.transform.scale[0]    << ", " << i.transform.scale[1]    << ", " << i.transform.scale[2]    << std::endl;
			std::cout << "[FlexData]	-Vertices:        " << i.vertexBuffer.size()   << std::endl;
			std::cout << "[FlexData]	-Indices:         " << i.indexBuffer.size()    << std::endl;
			std::cout << "[FlexData]    -Material index:  " << i.materialIndex         << std::endl;
			std::cout << "[FlexData]    -Bounding sphere:" << std::endl;
			std::cout << "[FlexData] 	    -Position: " << i.boundingSphere.position[0] << ", " << i.boundingSphere.position[1] << ", " << i.boundingSphere.position[2] << std::endl;
			std::cout << "[FlexData] 	    -Radius:   " << i.boundingSphere.radius      << std::endl;
			std::cout << "[FlexData]    -AABB:" << std::endl;
			std::cout << "[FlexData] 	    -Minimum: " << i.axisAlignedBoundingBox.minimum[0] << ", " << i.axisAlignedBoundingBox.minimum[1] << ", " << i.axisAlignedBoundingBox.minimum[2] << std::endl;
			std::cout << "[FlexData] 	    -Maximum: " << i.axisAlignedBoundingBox.maximum[0] << ", " << i.axisAlignedBoundingBox.maximum[1] << ", " << i.axisAlignedBoundingBox.maximum[2] << std::endl;
			if(printVertexData)
			{
				for(auto &j : i.vertexBuffer)
				{
					std::cout << std::endl;
					std::cout << "[FlexData]    -Vertex:" << std::endl;
					std::cout << "[FlexData] 	    -Position: " << j.position[0] << ", " << j.position[1] << ", " << j.position[2] << std::endl;
					std::cout << "[FlexData] 	    -UV:       " << j.uv[0]       << ", " << j.uv[1]       << std::endl;
					std::cout << "[FlexData] 	    -Normal:   " << j.normal[0]   << ", " << j.normal[1]   << ", " << j.normal[2]   << std::endl;
					std::cout << "[FlexData] 	    -Tangent:  " << j.tangent[0]  << ", " << j.tangent[1]  << ", " << j.tangent[2]  << std::endl;
				}

				for(auto &j : i.indexBuffer)
					std::cout << "[FlexData] Index: " << j << std::endl;
			}
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
			std::cout << "[FlexData]	-Albedo texture     | Width: " << i.albedo.width     << " | Height: " << i.albedo.height     << " | First pixel (XYZW): " << +i.albedo.data[0]     << ", " << +i.albedo.data[1] << ", " << +i.albedo.data[2] << ", " << +i.albedo.data[3] << std::endl;
			std::cout << "[FlexData]	-Normal texture     | Width: " << i.normal.width     << " | Height: " << i.normal.height     << " | First pixel (XYZW): " << +i.normal.data[0]     << ", " << +i.normal.data[1] << ", " << +i.normal.data[2] << ", " << +i.normal.data[3] << std::endl;
			std::cout << "[FlexData]	-Roughness texture  | Width: " << i.roughness.width  << " | Height: " << i.roughness.height  << " | First pixel (X): "    << +i.roughness.data[0]  << std::endl;
			std::cout << "[FlexData]	-Metalness texture  | Width: " << i.metalness.width  << " | Height: " << i.metalness.height  << " | First pixel (X): "    << +i.metalness.data[0]  << std::endl;
			std::cout << "[FlexData]	-Emissivity texture | Width: " << i.emissivity.width << " | Height: " << i.emissivity.height << " | First pixel (X): "    << +i.emissivity.data[0] << std::endl;
		}
		std::cout << std::endl;
	}
}