#pragma once

#include "Gogaman/Graphics/VertexBuffer.h"
#include "Gogaman/Graphics/IndexBuffer.h"

#include <glad.h>

namespace Gogaman
{
	class TestCube
	{
	public:
		TestCube()
		{
			glGenVertexArrays(1, &m_VAO);
			glBindVertexArray(m_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, m_VBO.GetRendererID());
			m_VBO.UploadData(sizeof(float) * m_Vertices.size(), m_Vertices.data());
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO.GetRendererID());
			m_IBO.UploadData(sizeof(uint16_t) * m_Indices.size(), m_Indices.data());
			
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
			glEnableVertexAttribArray(0);
			
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		~TestCube()
		{
			glDeleteVertexArrays(1, &m_VAO);
		}

		void Render() const
		{
			glBindVertexArray(m_VAO);
			glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_SHORT, 0);
		}
	private:
		GLuint       m_VAO;
		VertexBuffer m_VBO;
		IndexBuffer  m_IBO;

		const std::array<float, 12> m_Vertices = {
			 0.5f,  0.5f, 0.0f,  // top right
			 0.5f, -0.5f, 0.0f,  // bottom right
			-0.5f, -0.5f, 0.0f,  // bottom left
			-0.5f,  0.5f, 0.0f   // top left 
		};

		//const uint16_t m_Indices[6] = {
		//const unsigned int m_Indices[6] = {
		const std::array<unsigned short, 6> m_Indices = {
			0, 1, 3,
			1, 2, 3
		};
	};
}