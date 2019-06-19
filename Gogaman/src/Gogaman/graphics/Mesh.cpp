#include "pch.h"
#include "Mesh.h"

namespace Gogaman
{
	Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<ModelTexture> &textures)
			: m_Hidden(false), m_VBO(0), m_EBO(0), m_VAO(0), m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
	{
		//Configure VBO, VAO, and EBO
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1,      &m_VBO);
		glGenBuffers(1,      &m_EBO);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

		//Vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		//Vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		//Vertex texture coordinates
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		//Vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

		//Unbind VAO
		glBindVertexArray(0);
	}

	Mesh::~Mesh()
	{
		//glDeleteBuffers(1,      &m_VBO);
		//glDeleteBuffers(1,      &m_EBO);
		//glDeleteVertexArrays(1, &m_VAO);
	}

	void Mesh::Render(Shader &shader)
	{
		int diffuseNr    = 1;
		int roughnessNr  = 1;
		int normalNr     = 1;
		int heightNr     = 1;
		int metalnessNr  = 1;
		int emissivityNr = 1;

		//Bind texture(s) before rendering
		for(int i = 0; i < m_Textures.size(); i++)
		{
			//Activate texture unit before binding
			glActiveTexture(GL_TEXTURE1 + i);

			//Retrieve texture number
			std::string number;
			std::string name = m_Textures[i].type;

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
			glBindTexture(GL_TEXTURE_2D, m_Textures[i].id);
		}

		//Unbind texture
		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, 0);

		//Unbind VAO
		glBindVertexArray(0);
	}
}