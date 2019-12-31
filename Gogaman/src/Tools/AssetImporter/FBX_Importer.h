#pragma once

#include "FlexData.h"

#include <fbxsdk.h>

#include <glm.hpp>

namespace Gogaman
{
	namespace Tools
	{
		class FBX_Importer
		{
		public:
			FBX_Importer();
			~FBX_Importer();

			FlexData::FlexData Import(const char *filepath);
		private:
			void ProcessNodeRecursive(FlexData::FlexData &flexData, FbxNode *node);
			
			FbxAMatrix GetNodeTransform(FbxNode *node) const;

			template<typename GeometryElementType, typename ValueType>
			ValueType GetVertexElement(const GeometryElementType *element, const int controlPointIndex, const int triangle, const int vertex, const ValueType defaultValue) const;

			FlexData::FlexMeshData ProcessMesh(FbxNode *node);

			FlexData::FlexPointLightData ProcessPointLight(const FbxLight *light);
			FlexData::FlexDirectionalLightData ProcessDirectionalLight(const FbxLight *light);
		private:
			FbxManager    *m_SDK_Manager;
		};
	}
}