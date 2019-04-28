#include "pch.h"
#include "Texture2D.h"

namespace Gogaman
{
	Texture2D::Texture2D()
		: width(0), height(0), formatInternal(GL_RGB), formatImage(GL_RGB), wrapS(GL_REPEAT), wrapT(GL_REPEAT), filterMin(GL_NEAREST), filterMax(GL_NEAREST)
	{
		glGenTextures(1, &id);
	}

	Texture2D::~Texture2D()
	{}

	void Texture2D::Generate(GLuint width, GLuint height, unsigned char* data)
	{
		this->width = width;
		this->height = height;

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, formatInternal, width, height, 0, formatImage, GL_UNSIGNED_BYTE, data);
		//Set texture properties
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMax);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, id);
	}
}