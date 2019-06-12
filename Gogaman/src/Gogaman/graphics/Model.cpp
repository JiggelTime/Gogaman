#include "pch.h"
#include "Model.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Model::Model()
		: m_Hidden(false)
	{}

	Model::~Model()
	{}

	void Model::Render(Shader &shader, bool setPreviousModelMatrixUniform)
	{
		if(modelMatrixShouldUpdate)
			UpdateModelMatrix();

		modelMatrixShouldUpdate = false;

		shader.SetUniformMat4("M", modelMatrix);
		if(setPreviousModelMatrixUniform)
			shader.SetUniformMat4("previousM", previousModelMatrix);

		for(auto i : meshes)
		{
			if(!i.IsHidden())
				i.Render(shader);
		}
	}
	
	void Model::LoadModel(std::string &filePath)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			GM_LOG_CORE_ERROR("ASSIMP: %s", importer.GetErrorString());
		}

		directory = filePath.substr(0, filePath.find_last_of('/'));

		//Process the root node
		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode *node, const aiScene *scene)
	{
		//Process mesh at current node
		for(uint i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene));
		}

		//Process children nodes
		for(uint i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
	{
		std::vector<Vertex> vertices;
		std::vector<uint> indices;
		std::vector<ModelTexture> textures;

		//Process vertices
		for(uint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			glm::vec3 position;
			position.x = mesh->mVertices[i].x;
			position.y = mesh->mVertices[i].y;
			position.z = mesh->mVertices[i].z;
			vertex.position = position;

			glm::vec3 normal;
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;
			vertex.normal = normal;

			if(mesh->mTextureCoords[0])
			{
				glm::vec2 texCoords;
				texCoords.x = mesh->mTextureCoords[0][i].x;
				texCoords.y = mesh->mTextureCoords[0][i].y;
				vertex.texCoords = texCoords;
			}
			else
				vertex.texCoords = glm::vec2(0.0f, 0.0f);

			glm::vec3 tangent;
			tangent.x = mesh->mTangents[i].x;
			tangent.y = mesh->mTangents[i].y;
			tangent.z = mesh->mTangents[i].z;
			vertex.tangent = tangent;

			vertices.push_back(vertex);
		}

		//Process indices
		for(uint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for(uint j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		//Process textures
		if(mesh->mMaterialIndex >= 0)
		{
			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

			//Diffuse
			std::vector<ModelTexture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			//Specular
			std::vector<ModelTexture> roughnessMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_roughness");
			textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

			//Normal
			std::vector<ModelTexture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			//Height
			std::vector<ModelTexture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			//Metalness
			std::vector<ModelTexture> metalnessMaps = LoadMaterialTextures(material, aiTextureType_SHININESS, "texture_metalness");
			textures.insert(textures.end(), metalnessMaps.begin(), metalnessMaps.end());

			//Emissivity
			std::vector<ModelTexture> emissivityMaps = LoadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissivity");
			textures.insert(textures.end(), emissivityMaps.begin(), emissivityMaps.end());

			//Return the mesh object
			return Mesh(vertices, indices, textures);
		}
	}

	std::vector<ModelTexture> Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
	{
		std::vector<ModelTexture> textures;
		for(uint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for(uint j = 0; j < textures_loaded.size(); j++)
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
				ModelTexture texture;
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

	uint Model::LoadTextureFromFile(const char *path, const std::string &directory, bool gamma)
	{
		std::string filename = directory + '/' + std::string(path);

		uint textureID;
		glGenTextures(1, &textureID);

		int width, height, channels;
		
		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
		if(data)
		{
			GLenum format;
			if (channels == 1)
				format = GL_RED;
			else if (channels == 4)
				format = GL_RGBA;
			else
				format = GL_RGB;

			glBindTexture(GL_TEXTURE_2D, textureID);
			if (gamma == true && channels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			else if (gamma == true && channels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			if (GL_ARB_texture_filter_anisotropic)
			{
				float maxAF = 0.0f;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAF);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAF);
			}

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at: " << path << std::endl;
			stbi_image_free(data);
		}
		
		return textureID;
	}

	void Model::Hide()
	{
		for(auto i : meshes)
			i.Hide();
		
		m_Hidden = true;
	}
	
	void Model::Unhide()
	{
		for(auto i : meshes)
			i.Unhide();

		m_Hidden = false;
	}
}