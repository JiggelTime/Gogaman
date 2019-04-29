#include "pch.h"
#include "Texture3D.h"

namespace Gogaman
{
	Texture3D::Texture3D()
		: width(0), height(0), depth(0), formatInternal(GL_RGB), formatImage(GL_RGB), wrapS(GL_REPEAT), wrapT(GL_REPEAT), wrapR(GL_REPEAT), filterMin(GL_NEAREST), filterMax(GL_NEAREST)
	{
		glGenTextures(1, &id);
	}

	Texture3D::~Texture3D()
	{}

	void Texture3D::Generate(GLuint width, GLuint height, GLuint depth, unsigned char *data)
	{
		this->width = width;
		this->height = height;

		glBindTexture(GL_TEXTURE_3D, id);
		glTexImage3D(GL_TEXTURE_3D, 0, formatInternal, width, height, depth, 0, formatImage, GL_UNSIGNED_BYTE, data);
		//Set texture properties
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrapT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrapR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filterMin);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filterMax);

		glBindTexture(GL_TEXTURE_3D, 0);
	}
}