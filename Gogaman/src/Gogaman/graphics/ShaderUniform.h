#pragma once

#include <stdint.h>
#include <string>

namespace Gogaman
{
	enum class ShaderDataType
	{
		Bool, Int, Float, Vec2, Vec3, Vec4, Mat2, Mat3, Mat4
	};

	class ShaderUniform
	{
	public:
		virtual ~ShaderUniform() = 0;

		//void SetData(const uint32_t shaderID, uint8_t *data);

		inline const std::string GetName() const { return m_Name; };
	private:
		std::string m_Name;
	};

	class ShaderUniformBool
	{
		
	private:

	};
}