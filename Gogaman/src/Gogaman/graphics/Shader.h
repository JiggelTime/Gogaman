#pragma once

#include <stdint.h>

namespace Gogaman
{
	class Shader
	{
	public:
		//Shader(const char *vertexShaderSource, const char *fragmentShaderSource, const char *geometryShaderSource = nullptr);
		//Shader(const char *computeShaderSource);
		Shader()
			: m_ID(0)
		{}

		Shader(const Shader &) = delete;
		Shader(Shader &&other) noexcept
			: m_ID(std::exchange(other.m_ID, 0))
		{}

		virtual ~Shader()
		{}

		Shader &operator=(const Shader &) = delete;
		Shader &operator=(Shader &&other) noexcept
		{
			std::swap(m_ID, other.m_ID);
			return *this;
		}

		static Shader *Create();

		virtual void Compile(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const std::string &geometryShaderSource = "") = 0;
		virtual void Compile(const std::string &computeShaderSource) = 0;

		inline virtual void Bind() const = 0;
	protected:
		uint32_t m_ID;
	};
}