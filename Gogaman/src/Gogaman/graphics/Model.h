#ifndef MODEL_H
#define MODEL_H

#include <glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>
#include "mesh.h"
#include "shader.h"

unsigned int LoadTextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model
{
public:
	//Model data
	std::vector<Mesh> meshes;
	std::string       directory;
	bool              gammaCorrection;

	Model(std::string const &path, bool gamma = false)
		: gammaCorrection(gamma)
	{
		loadModel(path);
		std::cout << "Loaded model at: " + path << std::endl;
	}

	void Draw(Shader &shader, bool alsoSetPreviousModelMatrixUniform = false)
	{
		if(modelMatrixShouldUpdate)
			UpdateModelMatrix();
		
		modelMatrixShouldUpdate = false;

		shader.setMat4("M", modelMatrix);
		if(alsoSetPreviousModelMatrixUniform)
			shader.setMat4("previousM", previousModelMatrix);

		for(unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

	void UpdateModelMatrix()
	{
		previousModelMatrix = modelMatrix;
		modelMatrix = glm::mat4();

		//Apply translation
		if(position != glm::vec3(0.0f))
			modelMatrix = glm::translate(modelMatrix, position);
		//Apply rotation
		if(rotationAngle != 0.0f)
			modelMatrix = glm::rotate(modelMatrix,    rotationAngle, rotation);
		//Apply scale
		if(scale != glm::vec3(1.0f))
			modelMatrix = glm::scale(modelMatrix,     scale);
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

	glm::vec3 GetScale()
	{ return scale; }

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

	glm::vec3 GetPosition()
	{ return position; }

	void Translate(glm::vec3 translation)
	{
		this->position += translation;
		modelMatrixShouldUpdate = true;
	}

	void SetDynamic(bool isDynamic)
	{ this->isDynamic = isDynamic; }

	bool IsDynamic()
	{ return isDynamic; }
private:
	//Model properties
	glm::vec3 scale = glm::vec3(1.0f), rotation, position;
	float rotationAngle;
	//Dynamic or static
	bool isDynamic = false;

	glm::mat4 modelMatrix;
	glm::mat4 previousModelMatrix;
	bool modelMatrixShouldUpdate = false;

	std::vector<Texture> textures_loaded;

	//Loads model from file and stores resulting meshes in the meshes vector
	void loadModel(std::string const &path)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

		//Check for errors
		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ASSIMP: Error " << importer.GetErrorString() << std::endl;
		}

		//Retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		//Process the root node
		processNode(scene->mRootNode, scene);
	}

	//Recursively proccesses a node
	void processNode(aiNode *node, const aiScene *scene)
	{
		//Process each mesh located at the current node
		for(unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		//Process children nodes
		for(unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		//Process vertices
		for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			
			//Placeholder vector
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

unsigned int LoadTextureFromFile(const char *path, const std::string &directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;
	
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;

	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);	
	if(data)
	{
		GLenum format;
		if(nrComponents == 1)
			format = GL_RED;
		else if(nrComponents == 3)
			format = GL_RGB;
		else if(nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		if(gamma == true && nrComponents == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		else if(gamma == true && nrComponents == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if(GL_ARB_texture_filter_anisotropic)
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
#endif