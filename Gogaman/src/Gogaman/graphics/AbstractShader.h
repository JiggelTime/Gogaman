#pragma once

#include "Gogaman/CRTP.h"

namespace Gogaman
{
	using ShaderID = uint8_t;

	template<typename ShaderType>
	class AbstractShader : public CRTP<ShaderType, AbstractShader>
	{
	public:
		inline void Compile(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const std::string &geometryShaderSource = "")
		{
			this->GetImplementation().Compile(vertexShaderSource, fragmentShaderSource, geometryShaderSource);
		}

		inline void Compile(const std::string &computeShaderSource)
		{
			this->GetImplementation().Compile(computeShaderSource);
		}

		inline void Bind() const
		{
			this->GetImplementation().Bind();
		}
	protected:
		~AbstractShader() = default;
	public:
		ShaderID identifier;
	};
}