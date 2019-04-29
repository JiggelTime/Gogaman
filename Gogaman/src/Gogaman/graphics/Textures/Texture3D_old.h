#pragma once

#include "Gogaman/Core.h"
#include "../Shader.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API Texture3D
	{
	public:
		unsigned char* textureBuffer = nullptr;
		GLuint textureID;

		void Activate(Shader shaderProgram, const std::string glSamplerName, const int textureUnit = GL_TEXTURE0);

		void Clear(GLfloat clearColor[4]);

		Texture3D(const int _width, const int _height, const int _depth, const bool generateMipmaps, const GLenum _formatInternal = GL_RGBA8, const GLenum _format = GL_RGBA, const GLenum _type = GL_UNSIGNED_BYTE, const int _levels = 0, const GLenum _filterMin = GL_LINEAR_MIPMAP_LINEAR, const GLenum _filterMag = GL_LINEAR);
	private:
		int width, height, depth, levels;
		GLenum formatInternal, format, type, filterMin, filterMag;
		std::vector<GLfloat> clearData;
	};
}