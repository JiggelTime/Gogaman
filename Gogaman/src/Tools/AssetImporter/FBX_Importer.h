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
			Value                     GetVertexElement(const GeometryElement *element, const int controlPointIndex, const int triangle, const int vertex, const Value defaultValue) const;
			FlexData::FlexTextureData ProcessTexture(const FbxFileTexture *fileTexture, const uint8_t exportChannels, const FlexData::FlexTextureData &defaultTexture) const;
			uint32_t                  ProcessMaterial(const FbxNode *node);
			void                      ProcessMesh(FbxNode *node);
			void                      ProcessLight(const FbxLight *light);
			void                      ProcessNodeRecursive(FbxNode *node);
		private:
			mutable std::unordered_map<std::string, FlexData::FlexTextureData> m_Textures;

			FbxManager                                                        *m_SDK_Manager;
		};
	}
}