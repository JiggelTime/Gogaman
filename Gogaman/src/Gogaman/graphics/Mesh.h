#pragma once

#include "Gogaman/Core.h"
#include "Shader.h"
#include <glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace Gogaman
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
	};

	struct Texture
	{
		unsigned int id;
		std::string type;
		std::string path;
	};

	class GOGAMAN_API Mesh
	{
	public:
		//Mesh data
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		unsigned int VAO;

		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
		{
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;

			setupMesh();
		}

		void Render(Shader shader)
		{
			unsigned int diffuseNr = 1;
			unsigned int roughnessNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
			unsigned int metalnessNr = 1;
			unsigned int emissivityNr = 1;

			//Bind texture(s) before rendering
			for(unsigned int i = 0; i < textures.size(); i++)
			{
				//Activate texture unit before binding
				glActiveTexture(GL_TEXTURE1 + i);

				//Retrieve texture number
				std::string number;
				std::string name = textures[i].type;

				if(name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if(name == "texture_roughness")
					number = std::to_string(roughnessNr++);
				else if(name == "texture_normal")
					number = std::to_string(normalNr++);
				else if(name == "texture_height")
					number = std::to_string(heightNr++);
				else if(name == "texture_metalness")
					number = std::to_string(metalnessNr++);
				else if(name == "texture_emissivity")
					number = std::to_string(emissivityNr++);

				//Set sampler to the texture unit
				shader.setInt((name + number), i + 1);

				//Bind the texture
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
			}

			//Unbind texture
			glActiveTexture(GL_TEXTURE0);

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

			//Unbind VAO
			glBindVertexArray(0);
		}

	private:
		//Render data
		unsigned int VBO, EBO;

		//Functions
		void setupMesh()
		{
			//Configure VBO, VAO, and EBO
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			//Vertex positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

			//Vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

			//Vertex texture coordinates
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

			//Vertex tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

			//Unbind VAO
			glBindVertexArray(0);
		}
	};
}