#include "FBX_Importer.h"

#include <iostream>
#include <string>
#include <limits>
#include <unordered_map>

#define GM_FLIP_UV 0

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

			m_SDK_Manager->SetIOSettings(FbxIOSettings::Create(m_SDK_Manager, IOSROOT));
		}

		FBX_Importer::~FBX_Importer()
		{
			m_SDK_Manager->Destroy();
		}

		FlexData::FlexData FBX_Importer::Import(const char *filepath)
		{
			FbxImporter *importer = FbxImporter::Create(m_SDK_Manager, "");
			FbxScene *scene = FbxScene::Create(m_SDK_Manager, "Scene");

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
				std::cerr << "Message: " << importer->GetStatus().GetErrorString() << std::endl;
			}

			if(!importer->Import(scene))
				std::cerr << "Failed to import FBX scene" << std::endl;

			importer->Destroy();

			//Convert scene units to meters
			if(scene->GetGlobalSettings().GetSystemUnit() != FbxSystemUnit::m)
				FbxSystemUnit::m.ConvertScene(scene);

			//Convert scene axis system to MayaYUp
			if(scene->GetGlobalSettings().GetAxisSystem() != FbxAxisSystem::MayaYUp)
				FbxAxisSystem::MayaYUp.ConvertScene(scene);

			//Triangulate scene geometry
			FbxGeometryConverter geometryConverter(m_SDK_Manager);
			geometryConverter.Triangulate(scene, true);

			FbxNode *sceneRootNode = scene->GetRootNode();
			if(!sceneRootNode)
				std::cerr << "Failed to import FBX scene | FBX scene root node does not exist" << std::endl;

			FlexData::FlexData flexData;
			ProcessNodeRecursive(flexData, sceneRootNode);
			return flexData;
		}

		void FBX_Importer::ProcessNodeRecursive(FlexData::FlexData &flexData, FbxNode *node)
		{
			FbxNodeAttribute *attribute = node->GetNodeAttribute();
			if(attribute)
			{
				switch(attribute->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					flexData.meshes.emplace_back(ProcessMesh(node));
					break;
				case FbxNodeAttribute::eLight:
					if(node->GetLight()->LightType.Get() == FbxLight::ePoint)
						flexData.pointLights.emplace_back(ProcessPointLight(node->GetLight()));
					else if(node->GetLight()->LightType.Get() == FbxLight::eDirectional)
						flexData.directionalLights.emplace_back(ProcessDirectionalLight(node->GetLight()));
					break;
				}
			}

			for(int i = 0; i < node->GetChildCount(); i++)
				ProcessNodeRecursive(flexData, node->GetChild(i));
		}

		FbxAMatrix FBX_Importer::GetNodeTransform(FbxNode *node) const
		{
			FbxAMatrix geometryTransform;
			geometryTransform.SetIdentity();
			if(node->GetNodeAttribute())
			{
				FbxVector4 geometryTranslation = node->GetGeometricTranslation(FbxNode::eSourcePivot);
				FbxVector4 geometryRotation    = node->GetGeometricRotation(FbxNode::eSourcePivot);
				FbxVector4 geometryScale       = node->GetGeometricScaling(FbxNode::eSourcePivot);

				geometryTransform.SetT(geometryTranslation);
				geometryTransform.SetR(geometryRotation);
				geometryTransform.SetS(geometryScale);
			}

			FbxAMatrix globalTransform = node->EvaluateGlobalTransform();
			return globalTransform * geometryTransform;
		}

		template<typename GeometryElementType, typename ValueType>
		ValueType FBX_Importer::GetVertexElement(const GeometryElementType *element, const int controlPointIndex, const int triangle, const int vertex, const ValueType defaultValue) const
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

		FlexData::FlexMeshData FBX_Importer::ProcessMesh(FbxNode *node)
		{
			FbxMesh *mesh = node->GetMesh();

			if(!mesh->GenerateNormals())
			{
				std::cerr << "Failed to generate mesh normals" << std::endl;
				exit(EXIT_FAILURE);
			}

			if(!mesh->GenerateTangentsDataForAllUVSets())
			{
				std::cerr << "Failed to generate mesh tangents" << std::endl;
				exit(EXIT_FAILURE);
			}

			FlexData::FlexMeshData meshData;

			meshData.boundingSphere.position[0] = 0.0f;
			meshData.boundingSphere.position[1] = 0.0f;
			meshData.boundingSphere.position[2] = 0.0f;
			meshData.boundingSphere.radius      = 0.0f;

			meshData.axisAlignedBoundingBox.minimum[0] = FLT_MAX;
			meshData.axisAlignedBoundingBox.minimum[1] = FLT_MAX;
			meshData.axisAlignedBoundingBox.minimum[2] = FLT_MAX;
			meshData.axisAlignedBoundingBox.maximum[0] = 0.0f;
			meshData.axisAlignedBoundingBox.maximum[1] = 0.0f;
			meshData.axisAlignedBoundingBox.maximum[2] = 0.0f;

			uint32_t numTriangles = mesh->GetPolygonCount();
			meshData.vertexBuffer.reserve(3 * numTriangles);
			meshData.indexBuffer.reserve(3 * numTriangles);

			std::unordered_map<FlexData::FlexVertexData, uint16_t, FlexData::HashFlexVertexData> vertexIndices;
			
			//auto UpdateBoundingSphere = [](FlexData::FlexSphereData &boundingSphere, const glm::vec3 &point) {};

			for(uint32_t i = 0; i < numTriangles; i++)
			{
				for(uint32_t j = 0; j < 3; j++)
				{
					int        controlPointIndex = mesh->GetPolygonVertex(i, j);
					FbxVector4 controlPoint      = mesh->GetControlPointAt(controlPointIndex);
					
					FlexData::FlexVertexData vertex;
					//Position
					vertex.position[0] = (float)controlPoint[0];
					vertex.position[1] = (float)controlPoint[1];
					vertex.position[2] = (float)controlPoint[2];
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
					vertex.normal[0]   = (float)normal[0];
					vertex.normal[1]   = (float)normal[1];
					vertex.normal[2]   = (float)normal[2];
					//Tangent
					FbxVector4 tangent = GetVertexElement(mesh->GetElementTangent(0), controlPointIndex, i, j, FbxVector4(0.0f, 0.0f, 0.0f, 0.0f));
					vertex.tangent[0]  = (float)tangent[0];
					vertex.tangent[1]  = (float)tangent[1];
					vertex.tangent[2]  = (float)tangent[2];

					auto iterator = vertexIndices.find(vertex);
					//Vertex not in index map (new vertex)
					if(iterator == vertexIndices.end())
					{
						if(meshData.vertexBuffer.size() >= UINT16_MAX)
						{
							std::cerr << "Failed to import FBX mesh: number of vertices exceeds " << UINT16_MAX - 1 << std::endl;
							exit(EXIT_FAILURE);
						}

						//AABB
						for(int i = 0; i < 3; i++)
						{
							//Minimum
							if(vertex.position[i] < meshData.axisAlignedBoundingBox.minimum[i])
								meshData.axisAlignedBoundingBox.minimum[i] = vertex.position[i];
							//Maximum
							if(vertex.position[i] > meshData.axisAlignedBoundingBox.maximum[i])
								meshData.axisAlignedBoundingBox.maximum[i] = vertex.position[i];
						}
						
						uint16_t index = (uint16_t)meshData.vertexBuffer.size();
						meshData.vertexBuffer.emplace_back(std::move(vertex));
						meshData.indexBuffer.emplace_back(index);
						vertexIndices[vertex] = index;
					}
					//Vertex already in index map (duplicate vertex)
					else
						meshData.indexBuffer.emplace_back(iterator->second);
				}
			}

			FbxAMatrix meshTransform = GetNodeTransform(mesh->GetNode());
			//Position
			FbxVector4 meshPosition = meshTransform.GetT();
			for(uint8_t i = 0; i < 3; i++)
			{
				meshData.transform.position[i] = (float)meshPosition[i];
			}
			//Rotation
			FbxVector4 meshRotation = meshTransform.GetR();
			for(uint8_t i = 0; i < 4; i++)
			{
				meshData.transform.rotation[i] = (float)meshRotation[i];
			}
			//TODO: Fix rotation. Hardcoded to 0 to disable for now
			meshData.transform.rotation[3] = 0.0f;
			//Scale
			FbxVector4 meshScale = meshTransform.GetS();
			for(uint8_t i = 0; i < 3; i++)
			{
				meshData.transform.scale[i] = (float)meshScale[i];
			}

			//Bounding sphere
			//TODO: Use Matoušek, Sharir, Welzl's algorithm
			float axisAlignedBoundingBoxMean[3];
			for(int i = 0; i < 3; i++)
			{
				axisAlignedBoundingBoxMean[i]       = (meshData.axisAlignedBoundingBox.maximum[i] - meshData.axisAlignedBoundingBox.minimum[i]) / 2.0f;
				meshData.boundingSphere.position[i] = meshData.axisAlignedBoundingBox.minimum[i] + axisAlignedBoundingBoxMean[i];
			}

			meshData.boundingSphere.radius = axisAlignedBoundingBoxMean[0] > axisAlignedBoundingBoxMean[1] ? axisAlignedBoundingBoxMean[0] : (axisAlignedBoundingBoxMean[1] > axisAlignedBoundingBoxMean[2] ? axisAlignedBoundingBoxMean[1] : axisAlignedBoundingBoxMean[2]);

			return meshData;
		}

		FlexData::FlexPointLightData FBX_Importer::ProcessPointLight(const FbxLight *light)
		{
			FlexData::FlexPointLightData pointLightData;
			//Position
			FbxVector4 position = light->GetNode()->EvaluateGlobalTransform().GetT();
			pointLightData.position[0] = (float)position[0];
			pointLightData.position[1] = (float)position[1];
			pointLightData.position[2] = (float)position[2];
			//Radiance
			FbxDouble3 color     = light->Color.Get();
			float      intensity = (float)light->Intensity.Get();
			pointLightData.radiance[0] = (float)color[0] * intensity;
			pointLightData.radiance[1] = (float)color[1] * intensity;
			pointLightData.radiance[2] = (float)color[2] * intensity;

			return pointLightData;
		}

		FlexData::FlexDirectionalLightData FBX_Importer::ProcessDirectionalLight(const FbxLight *light)
		{
			FlexData::FlexDirectionalLightData directionalLightData;
			//Direction
			FbxVector4 direction = light->GetNode()->EvaluateGlobalTransform().GetR();
			directionalLightData.direction[0] = (float)direction[0];
			directionalLightData.direction[1] = (float)direction[1];
			directionalLightData.direction[2] = (float)direction[2];
			//Radiance
			FbxDouble3 color     = light->Color.Get();
			float      intensity = (float)light->Intensity.Get();
			directionalLightData.radiance[0] = (float)color[0] * intensity;
			directionalLightData.radiance[1] = (float)color[1] * intensity;
			directionalLightData.radiance[2] = (float)color[2] * intensity;

			return directionalLightData;
		}
	}
}