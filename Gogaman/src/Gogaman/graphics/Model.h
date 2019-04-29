#pragma once

#include "Gogaman/Core.h"
#include "Mesh.h"
#include "Shader.h"

#include <glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

namespace Gogaman
{
	class GOGAMAN_API Model
	{
	public:
		//Model data
		std::vector<Mesh> meshes;
		std::string       directory;
		bool              gammaCorrection = false;

		Model();
		~Model();

		//Loads model from file and stores resulting meshes in the meshes vector
		void LoadModel(std::string &filePath);

		void Render(Shader &shader, bool setPreviousModelMatrixUniform = false);

		void UpdateModelMatrix()
		{
			previousModelMatrix = modelMatrix;
			modelMatrix = glm::mat4();

			//Apply translation
			if(position != glm::vec3(0.0f))
				modelMatrix = glm::translate(modelMatrix, position);
			//Apply rotation
			if(rotationAngle != 0.0f)
				modelMatrix = glm::rotate(modelMatrix, rotationAngle, rotation);
			//Apply scale
			if(scale != glm::vec3(1.0f))
				modelMatrix = glm::scale(modelMatrix, scale);
		}

		void SetScale(glm::vec3 scale)
		{
			this->scale = scale;
			modelMatrixShouldUpdate = true;
		}

		void SetScale(float scale)
		{
			this->scale = glm::vec3(scale);
			modelMatrixShouldUpdate = true;
		}

		inline glm::vec3 GetScale() { return scale; }

		void Scale(glm::vec3 scale)
		{
			this->scale *= scale;
			modelMatrixShouldUpdate = true;
		}

		void Scale(float scale)
		{
			this->scale *= glm::vec3(scale);
			modelMatrixShouldUpdate = true;
		}

		void Rotate(float rotationAngle, glm::vec3 rotation)
		{
			this->rotationAngle = rotationAngle;
			this->rotation = rotation;
			modelMatrixShouldUpdate = true;
		}

		void SetPosition(glm::vec3 position)
		{
			this->position = position;
			modelMatrixShouldUpdate = true;
		}

		glm::vec3 GetPosition() { return position; }

		void Translate(glm::vec3 translation)
		{
			this->position += translation;
			modelMatrixShouldUpdate = true;
		}

		void SetDynamic(bool isDynamic)
		{
			this->isDynamic = isDynamic;
		}

		bool IsDynamic()
		{
			return isDynamic;
		}
	private:
		//Model properties
		glm::vec3 scale = glm::vec3(1.0f), rotation, position;
		float rotationAngle = 0.0f;
		//Dynamic or static
		bool isDynamic = false;

		glm::mat4 modelMatrix;
		glm::mat4 previousModelMatrix;
		bool modelMatrixShouldUpdate = false;

		std::vector<Texture> textures_loaded;

		//Recursively proccesses a node
		void ProcessNode(aiNode *node, const aiScene *scene);

		Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);

		unsigned int LoadTextureFromFile(const char *path, const std::string &directory, bool gamma = false);

		std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
		{
			std::vector<Texture> textures;
			for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);
				bool skip = false;
				for(unsigned int j = 0; j < textures_loaded.size(); j++)
				{
					if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
					{
						textures.push_back(textures_loaded[j]);
						skip = true;
						break;
					}
				}

				//Load the texture if it hasn't already been loaded
				if(!skip)
				{
					Texture texture;
					if(typeName == "texture_diffuse")
						texture.id = LoadTextureFromFile(str.C_Str(), directory, true);
					else
						texture.id = LoadTextureFromFile(str.C_Str(), directory);
					texture.type = typeName;
					texture.path = str.C_Str();
					textures.push_back(texture);

					//Add to loaded textures
					textures_loaded.push_back(texture);
				}
			}

			return textures;
		}
	};
}