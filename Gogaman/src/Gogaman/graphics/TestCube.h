#pragma once

#include "Gogaman/Graphics/VertexBuffer.h"
//#include "Gogaman/Graphics/IndexBuffer.h"

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
			m_VBO.UploadData(sizeof(m_Vertices), m_Vertices);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
		}

		~TestCube()
		{
			glDeleteVertexArrays(1, &m_VAO);
		}

		void Render() const
		{
			glBindVertexArray(m_VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	private:
		GLuint       m_VAO;
		VertexBuffer m_VBO;

		const float m_Vertices[9] = {
		-0.5f, -0.5f, 0.0f, // left  
		 0.5f, -0.5f, 0.0f, // right 
		 0.0f,  0.5f, 0.0f  // top   
		};
	};
}