#include "pch.h"
#include "Model.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Model::Model(std::string &filePath)
	{
		LoadModel(filePath);
	}

	void Model::Render(Shader &shader, bool setPreviousModelMatrixUniform)
	{
		if(modelMatrixShouldUpdate)
			UpdateModelMatrix();

		modelMatrixShouldUpdate = false;

		shader.setMat4("M", modelMatrix);
		if(setPreviousModelMatrixUniform)
			shader.setMat4("previousM", previousModelMatrix);

		for(unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Render(shader);
	}

	void Model::LoadModel(std::string &filePath)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			GM_LOG_CORE_ERROR("ASSIMP: %s", importer.GetErrorString());
		}

		//Retrieve the directory path of the filepath
		directory = filePath.substr(0, filePath.find_last_of('/'));

		//Process the root node
		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode *node, const aiScene *scene)
	{
		//Process mesh at current node
		for(unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene));
		}

		//Process children nodes
		for(unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		//Process vertices
		for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector3;

			//Positions
			vector3.x = mesh->mVertices[i].x;
			vector3.y = mesh->mVertices[i].y;
			vector3.z = mesh->mVertices[i].z;
			vertex.Position = vector3;

			//Normals
			vector3.x = mesh->mNormals[i].x;
			vector3.y = mesh->mNormals[i].y;
			vector3.z = mesh->mNormals[i].z;
			vertex.Normal = vector3;

			//Texture coordinates
			if(mesh->mTextureCoords[0])
			{
				glm::vec2 vector2;
				vector2.x = mesh->mTextureCoords[0][i].x;
				vector2.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vector2;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			//Tangents
			vector3.x = mesh->mTangents[i].x;
			vector3.y = mesh->mTangents[i].y;
			vector3.z = mesh->mTangents[i].z;
			vertex.Tangent = vector3;

			vertices.push_back(vertex);
		}

		//Process indices
		for(unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			//Retrieve the face's indices and store them in the indices vector
			for(unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		//Process textures
		if(mesh->mMaterialIndex >= 0)
		{
			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

			//Diffuse
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			//Specular
			std::vector<Texture> roughnessMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_roughness");
			textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

			//Normal
			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			//Height
			std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			//Metalness
			std::vector<Texture> metalnessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "texture_metalness");
			textures.insert(textures.end(), metalnessMaps.begin(), metalnessMaps.end());

			//Emissivity
			std::vector<Texture> emissivityMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissivity");
			textures.insert(textures.end(), emissivityMaps.begin(), emissivityMaps.end());

			//Return the mesh object
			return Mesh(vertices, indices, textures);
		}
	}
}