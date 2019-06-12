#pragma once

#include "Gogaman/Graphics/ShaderUniform.h"

#include <glad.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	class OpenGL_ShaderUniformBool : public ShaderUniformBool
	{
		inline virtual void SetData(const std::string &name, const uint32_t shaderID, uint8_t *data) const override
		{
			glProgramUniform1i(shaderID, glGetUniformLocation(shaderID, name.c_str()), (int)data);
		}
	};
}