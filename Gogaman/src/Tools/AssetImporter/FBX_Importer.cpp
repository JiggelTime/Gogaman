#include "FBX_Importer.h"

#include <iostream>
#include <string>

namespace Gogaman
{
	namespace Tools
	{
		FBX_Importer::FBX_Importer()
		{
			m_SDK_Manager = FbxManager::Create();
			if(!m_SDK_Manager)
			{
				std::cerr << "Failed to create FBX SDK manager" << std::endl;
				return;
			}
			else
				std::cout << "Initialized Autodesk FBX SDK | Version: " << m_SDK_Manager->GetVersion() << std::endl;
		}

		FBX_Importer::~FBX_Importer()
		{
			m_SDK_Manager->Destroy();
		}

		FlexData::FlexData FBX_Importer::Import(const char *filepath)
		{
			FbxIOSettings *ioSettings = FbxIOSettings::Create(m_SDK_Manager, IOSROOT);
			m_SDK_Manager->SetIOSettings(ioSettings);

			FbxImporter *importer = FbxImporter::Create(m_SDK_Manager, "");
			FbxScene    *scene    = FbxScene::Create(m_SDK_Manager, "Scene");

			if(importer->Initialize(filepath, -1, m_SDK_Manager->GetIOSettings()))
			{
				std::cout << "Importing FBX file | Location: " << filepath << std::endl;

				int fileVersionMajor, fileVersionMinor, fileVersionRevision;
				importer->GetFileVersion(fileVersionMajor, fileVersionMinor, fileVersionRevision);
				std::cout << "File FBX version: " << fileVersionMajor << "." << fileVersionMinor << "." << fileVersionRevision << std::endl;
			}
			else
			{
				std::cerr << "Failed to load FBX file | Location: " << filepath << std::endl;
				std::cerr << "Error: " << importer->GetStatus().GetErrorString() << std::endl;
			}

			if(!importer->Import(scene))
				std::cerr << "Failed to import FBX scene" << std::endl;

			importer->Destroy();

			//Convert scene units to meters
			if(scene->GetGlobalSettings().GetSystemUnit() != FbxSystemUnit::m)
				FbxSystemUnit::m.ConvertScene(scene);
			
			//Triangulate scene geometry
			FbxGeometryConverter geometryConverter(m_SDK_Manager);
			geometryConverter.Triangulate(scene, true);

			FbxNode *sceneRootNode = scene->GetRootNode();
			if(sceneRootNode)
				ProcessNodeRecursive(sceneRootNode);
			
			return m_FlexData;
		}

		template<typename GeometryElement, typename Value>
		Value FBX_Importer::GetVertexElement(GeometryElement *element, int controlPointIndex, int triangle, int vertex, Value defaultValue)
		{
			int index = 0;
			if(!element)
				return defaultValue;

			switch(element->GetMappingMode())
			{
			case FbxGeometryElement::eByControlPoint:
				index = controlPointIndex;
				break;
			case FbxGeometryElement::eByPolygon:
				index = triangle;
				break;
			case FbxGeometryElement::eByPolygonVertex:
				index = triangle * 3 + vertex;
				break;
			default:
				return defaultValue;
			}

			if(element->GetReferenceMode() != FbxGeometryElement::eDirect)
				index = element->GetIndexArray().GetAt(index);

			return element->GetDirectArray().GetAt(index);
		}

		void FBX_Importer::ProcessMesh(FbxMesh *mesh)
		{
			if(!mesh->GetElementNormal(0))
				mesh->GenerateNormals();

			FlexData::FlexMeshData meshData;
			std::unordered_map<FlexData::FlexVertexData, uint16_t, FlexData::FlexVertexDataHashFunction> vertexIndices;

			uint32_t numTriangles = mesh->GetPolygonCount();
			meshData.vertexBufferData.reserve(3 * numTriangles);
			meshData.indexBufferData.reserve(3 * numTriangles);

			for(uint32_t i = 0; i < numTriangles; i++)
			{
				for(uint32_t j = 0; j < 3; j++)
				{
					int        controlPointIndex = mesh->GetPolygonVertex(i, j);
					FbxVector4 controlPoint      = mesh->GetControlPointAt(controlPointIndex);
					
					FlexData::FlexVertexData vertex;
					//Position
					vertex.position[0] = static_cast<float>(controlPoint[0]);
					vertex.position[1] = static_cast<float>(controlPoint[1]);
					vertex.position[2] = static_cast<float>(controlPoint[2]);
					//UV
					FbxVector2 uv      = GetVertexElement(mesh->GetElementUV(0), controlPointIndex, i, j, FbxVector2(0.0f, 1.0f));
					vertex.uv[0]       = static_cast<float>(uv[0]);
					#if GM_FLIP_UV
						vertex.uv[1]   = 1.0f - static_cast<float>(uv[1]);
					#else
						vertex.uv[1]   = static_cast<float>(uv[1]);
					#endif
					//Normal
					FbxVector4 normal  = GetVertexElement(mesh->GetElementNormal(0), controlPointIndex, i, j, FbxVector4(0.0f, 0.0f, 0.0f, 0.0f));
					vertex.normal[0]   = static_cast<float>(normal[0]);
					vertex.normal[1]   = static_cast<float>(normal[1]);
					vertex.normal[2]   = static_cast<float>(normal[2]);
					//Tangent
					FbxVector4 tangent = GetVertexElement(mesh->GetElementTangent(0), controlPointIndex, i, j, FbxVector4(0.0f, 0.0f, 0.0f, 0.0f));
					vertex.tangent[0]  = static_cast<float>(tangent[0]);
					vertex.tangent[1]  = static_cast<float>(tangent[1]);
					vertex.tangent[2]  = static_cast<float>(tangent[2]);

					auto iterator = vertexIndices.find(vertex);
					//Vertex is not in index map (new vertex)
					if(iterator == vertexIndices.end())
					{
						if(meshData.vertexBufferData.size() >= UINT16_MAX)
						{
							std::cout << "Failed to import FBX mesh: number of vertices exceeds " << UINT16_MAX << std::endl;
							exit(1);
						}
						
						uint16_t index = meshData.vertexBufferData.size();
						meshData.vertexBufferData.emplace_back(std::move(vertex));
						meshData.indexBufferData.emplace_back(index);
						vertexIndices[vertex] = index;
					}
					//Vertex is already in index map (duplicate vertex)
					else
						meshData.indexBufferData.emplace_back(iterator->second);
				}
			}

			m_FlexData.meshes.emplace_back(std::move(meshData));
		}

		void FBX_Importer::ProcessTexture(FbxFileTexture *fileTexture, bool sRGB)
		{
			if(!fileTexture)
				return;

			const char *filepath = fileTexture->GetFileName();

		}

		void FBX_Importer::ProcessMaterial(FbxSurfaceMaterial *material)
		{
			if(material == nullptr)
				return;


		}

		void FBX_Importer::ProcessNodeRecursive(FbxNode *node)
		{
			FbxNodeAttribute *attribute = node->GetNodeAttribute();
			if(attribute)
			{
				switch(attribute->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					ProcessMesh(node->GetMesh());
					break;
				case FbxNodeAttribute::eLight:
					break;
				}

				for(int i = 0; i < node->GetMaterialCount(); i++)
					ProcessMaterial(node->GetMaterial(i));
			}

			for(int i = 0; i < node->GetChildCount(); i++)
				ProcessNodeRecursive(node->GetChild(i));
		}
	}
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		//std::cerr << "Failed to run: too few arguments" << std::endl;
		//return -1;
	}

	//const char *importFilepath = argv[1];
	//const char *exportFilepath = argv[2];
	const char *importFilepath = "D:/dev/test.fbx";
	const char *exportFilepath = "D:/dev/test.flex";

	FlexData::FlexData testFlexData;
	//Import FBX file
	Gogaman::Tools::FBX_Importer importer;
	testFlexData = importer.Import(importFilepath);
	
	std::cout << "[FBX Importer] Number of meshes: " << testFlexData.meshes.size() << std::endl;
	for(auto i : testFlexData.meshes)
	{
		std::cout << std::endl;
		std::cout << "[FBX Importer] Mesh:" << std::endl;
		std::cout << "[FBX Importer]	-Number of vertices:      " << i.vertexBufferData.size() << std::endl;
		std::cout << "[FBX Importer]	-Number of indices:       " << i.indexBufferData.size()  << std::endl;
	}
	std::cout << std::endl;

	//Export FlexData
	FlexData::ExportFlexData(exportFilepath, testFlexData);

	//Import FlexData
	/*
	testFlexData = FlexData::ImportFlexData(exportFilepath);
	std::cout << "[FlexData Importer] Number of meshes: " << testFlexData.meshes.size() << std::endl;
	for(auto i : testFlexData.meshes)
	{
		std::cout << std::endl;
		std::cout << "[FlexData Importer] Mesh:" << std::endl;
		std::cout << "[FlexData Importer]	-Number of vertices:      " << i.vertexBufferData.size() << std::endl;
		std::cout << "[FlexData Importer]	-Number of indices:       " << i.indexBufferData.size()  << std::endl;
	}
	std::cout << std::endl;
	
	for(auto i : testFlexData.meshes)
	{
		for(auto j : i.vertexBufferData)
		{
			std::cout << std::endl;
			std::cout << "Vertex:" << std::endl;
			std::cout << "	-Position (xyz): " << j.position[0] << ", " << j.position[1] << ", " << j.position[2] << std::endl;
			std::cout << "	-UV       (xy):  " << j.uv[0]       << ", " << j.uv[1]       << std::endl;
			std::cout << "	-Normal   (xyz): " << j.normal[0]   << ", " << j.normal[1]   << ", " << j.normal[2]   << std::endl;
			std::cout << "	-Tangent  (xyz): " << j.tangent[0]  << ", " << j.tangent[1]  << ", " << j.tangent[2]  << std::endl;
		}

		for(auto j : i.indexBufferData)
			std::cout << "Index: " << j << std::endl;
	}
	*/
	return 0;
}