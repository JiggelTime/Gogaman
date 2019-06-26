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
			FbxAMatrix                GetNodeTransform(FbxNode *node) const;
			template<typename GeometryElement, typename Value>
			Value                     GetVertexElement(GeometryElement *element, int controlPointIndex, int triangle, int vertex, Value defaultValue) const;
			void                      ProcessMesh(FbxMesh *mesh);
			void                      ProcessLight(const FbxLight *light);
			FlexData::FlexTextureData ProcessTexture(const FbxFileTexture *fileTexture, const uint8_t exportChannels, const FlexData::FlexTextureData &defaultTexture);
			void                      ProcessMaterial(const FbxSurfaceMaterial *material);
			void                      ProcessNodeRecursive(FbxNode *node);
		private:
			FbxManager *m_SDK_Manager;
			std::unordered_map<std::string, FlexData::FlexTextureData> m_Textures;
		};
	}
}