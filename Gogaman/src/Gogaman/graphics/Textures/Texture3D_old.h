#pragma once

#include "Gogaman/Core.h"
#include "Gogaman/Graphics/Shader.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API Texture3D
	{
	public:
		Texture3D(int _width, int _height, int _depth, bool generateMipmaps, GLenum _formatInternal = GL_RGBA8, GLenum _format = GL_RGBA, GLenum _type = GL_UNSIGNED_BYTE, int _levels = 0, GLenum _filterMin = GL_LINEAR_MIPMAP_LINEAR, GLenum _filterMag = GL_LINEAR);
		Texture3D() {}
		void Activate(Shader shaderProgram, const std::string glSamplerName, const int textureUnit = GL_TEXTURE0);

		void Clear(GLfloat clearColor[4]);
	public:
		GLuint textureID;
		unsigned char *textureBuffer = nullptr;
	private:
		int width, height, depth, levels;
		GLenum formatInternal, format, type, filterMin, filterMag;
		std::vector<GLfloat> clearData;
	};
}