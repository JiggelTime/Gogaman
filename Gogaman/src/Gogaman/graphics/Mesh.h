#pragma once

#include "Gogaman/Base.h"
#include "Gogaman/Graphics/Shader.h"

#include <glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace Gogaman
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
		glm::vec3 tangent;
	};

	struct ModelTexture
	{
		unsigned int id;
		std::string type;
		std::string path;
	};

	class Mesh
	{
	public:
		Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<ModelTexture> &textures);
		~Mesh();

		void Render(Shader &shader);

		inline void Hide()   { m_Hidden = true; }
		inline void Unhide() { m_Hidden = false; }

		inline bool IsHidden() const { return m_Hidden; }
	private:
		bool m_Hidden;

		unsigned int m_VBO, m_EBO, m_VAO;

		std::vector<Vertex>       m_Vertices;
		std::vector<unsigned int>         m_Indices;
		std::vector<ModelTexture> m_Textures;
	};
}