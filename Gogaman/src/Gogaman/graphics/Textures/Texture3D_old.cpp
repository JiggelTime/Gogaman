#include "pch.h"
#include "Texture3D.h"

namespace Gogaman
{
	Texture3D::Texture3D(const int _width, const int _height, const int _depth, const bool generateMipmaps, const GLenum _formatInternal, const GLenum _format, const GLenum _type, const int _levels, const GLenum _filterMin, const GLenum _filterMag)
		: width(_width), height(_height), depth(_depth), clearData(4 * _width* _height* _depth, 0.0f), formatInternal(_formatInternal), format(_format), type(_type), levels(_levels), filterMin(_filterMin), filterMag(_filterMag)
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_3D, textureID);

		const auto wrap = GL_CLAMP_TO_BORDER;
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filterMin);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filterMag);

		glTexStorage3D(GL_TEXTURE_3D, levels, formatInternal, width, height, depth);
		glTexImage3D(GL_TEXTURE_3D, 0, formatInternal, width, height, depth, 0, format, type, &textureBuffer[0]);

		if(generateMipmaps)
			glGenerateMipmap(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, 0);
	}

	void Texture3D::Activate(Shader shaderProgram, const std::string glSamplerName, const int textureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_3D, textureID);
		shaderProgram.setInt(glSamplerName, textureUnit);
	}

	void Texture3D::Clear(GLfloat clearColor[4])
	{
		GLint previousBoundTextureID;
		glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousBoundTextureID);
		glBindTexture(GL_TEXTURE_3D, textureID);
		glClearTexImage(textureID, 0, format, GL_FLOAT, &clearColor);
		glBindTexture(GL_TEXTURE_3D, previousBoundTextureID);
	}
}