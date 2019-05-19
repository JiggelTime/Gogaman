#pragma once

#include "Gogaman/Core.h"
#include "Platform/OpenGL/Shader.h"
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

	struct ModelTexture
	{
		uint id;
		std::string type;
		std::string path;
	};

	class GOGAMAN_API Mesh
	{
	public:
		//Mesh data
		std::vector<Vertex> vertices;
		std::vector<uint> indices;
		std::vector<ModelTexture> textures;
		uint VAO;

		Mesh(std::vector<Vertex> &vertices, std::vector<uint> &indices, std::vector<ModelTexture> &textures)
			: vertices(vertices), indices(indices), textures(textures), m_Hidden(false)
		{
			setupMesh();
		}

		void Render(Shader &shader)
		{
			uint diffuseNr    = 1;
			uint roughnessNr  = 1;
			uint normalNr     = 1;
			uint heightNr     = 1;
			uint metalnessNr  = 1;
			uint emissivityNr = 1;

			//Bind texture(s) before rendering
			for(uint i = 0; i < textures.size(); i++)
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
				shader.SetUniformInt((name + number), i + 1);

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

		inline void Hide()   { m_Hidden = true; }
		inline void Unhide() { m_Hidden = false; }

		inline bool IsHidden() const { return m_Hidden; }
	private:
		bool m_Hidden;

		//Render data
		uint VBO, EBO;

		//Functions
		inline void setupMesh()
		{
			//Configure VBO, VAO, and EBO
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

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