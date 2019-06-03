#pragma once

#include "Gogaman/Base.h"
#include "Mesh.h"
#include "Platform/OpenGL/Shader.h"

#include <glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <stb_image.h>
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>

namespace Gogaman
{
	class Model
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

		//void Render(Shader &shader, bool setPreviousModelMatrixUniform = false);

		void UpdateModelMatrix()
		{
			if(previousModelMatrix != modelMatrix)
				SetDynamic(true);

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

		inline glm::vec3 GetScale() const { return scale; }

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

		glm::vec3 GetPosition() const { return position; }

		void Translate(glm::vec3 translation)
		{
			this->position += translation;
			modelMatrixShouldUpdate = true;
		}

		inline void SetDynamic(const bool &isDynamic)
		{ this->isDynamic = isDynamic; }

		inline bool IsDynamic() const
		{ return isDynamic; }

		//void Hide();
		//void Unhide();

		inline bool IsHidden() const { return m_Hidden; }
	private:
		//void ProcessNode(aiNode *node, const aiScene *scene);
		//Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
		//std::vector<ModelTexture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
		//uint LoadTextureFromFile(const char *path, const std::string &directory, bool gamma = false);
	private:
		//Model properties
		glm::vec3 scale = glm::vec3(1.0f), rotation, position;
		float rotationAngle = 0.0f;
		//Dynamic or static
		bool isDynamic = false;

		bool m_Hidden;

		glm::mat4 modelMatrix;
		glm::mat4 previousModelMatrix;
		bool modelMatrixShouldUpdate = false;

		std::vector<ModelTexture> textures_loaded;
	};
}