#pragma once

#include "Importer.h"

#include "FlexData.h"

#include <fbxsdk.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <unordered_map>

#define GM_FLIP_UV 1

namespace Gogaman
{
	namespace Tools
	{
		class FBX_Importer : public Importer
		{
		public:
			FBX_Importer();
			~FBX_Importer();

			virtual FlexData::FlexData Import(const char *filepath) override;
		private:
			template<typename GeometryElement, typename Value>
			Value GetVertexElement(GeometryElement *element, int controlPointIndex, int triangle, int vertex, Value defaultValue);
			void  ProcessMesh(FbxMesh *mesh);
			void  ProcessTexture(FbxFileTexture *fileTexture, bool sRGB);
			void  ProcessMaterial(FbxSurfaceMaterial *material);
			void  ProcessNodeRecursive(FbxNode *node);
		private:
			FbxManager *m_SDK_Manager;
			std::unordered_map<std::string, uint32_t> m_Textures;
		};
	}
}