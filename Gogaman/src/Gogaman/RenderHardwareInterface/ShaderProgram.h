#pragma once

#include "Gogaman/Core/Base.h"

#include "Identifier.h"

#include "Shader.h"

namespace Gogaman
{
	namespace RHI
	{
		class ShaderProgram
		{
		public:
			ShaderProgram()                      = default;
			ShaderProgram(const ShaderProgram &) = delete;
			ShaderProgram(ShaderProgram &&)      = default;

			~ShaderProgram() = default;

			ShaderProgram &operator=(const ShaderProgram &) = delete;
			ShaderProgram &operator=(ShaderProgram &&)      = default;

			template<Shader::Stage stage>
			inline void SetShader(const ShaderID shader)
			{
				GM_DEBUG_ASSERT(GM_IS_VALID_ID(shader), "Failed to set shader | Invalid shader");
				
				m_Shaders[(uint8_t)stage] = shader;
			}

			inline void SetShader(const Shader::Stage stage, const ShaderID shader)
			{
				GM_DEBUG_ASSERT(GM_IS_VALID_ID(shader), "Failed to set shader | Invalid shader");

				m_Shaders[(uint8_t)stage] = shader;
			}

			template<Shader::Stage stage>
			inline constexpr bool IsShaderPresent()                          const { return GM_IS_VALID_ID(m_Shaders[(uint8_t)stage]); }
			inline constexpr bool IsShaderPresent(const Shader::Stage stage) const { return GM_IS_VALID_ID(m_Shaders[(uint8_t)stage]); }

			template<Shader::Stage stage>
			inline constexpr ShaderID GetShader() const
			{
				GM_DEBUG_ASSERT(IsShaderPresent<stage>()), "Failed to get shader | No shader present at shader stage");

				return m_Shaders[(uint8_t)stage];
			}

			inline constexpr ShaderID GetShader(const Shader::Stage stage) const
			{
				GM_DEBUG_ASSERT(IsShaderPresent(stage), "Failed to get shader | No shader present at shader stage");

				return m_Shaders[(uint8_t)stage];
			}
		private:
			ShaderID m_Shaders[GM_RHI_SHADER_STAGE_COUNT];
		};
	}
}