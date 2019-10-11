#include "pch.h"
#include "OpenGL_Shader.h"

#define GM_GLSL_VERSION 450
#define GM_ENABLE_SHADER_VALIDATION 1

namespace Gogaman
{
	Shader::Shader()
	{
		m_RendererID = glCreateProgram();
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Shader::Compile(const std::string &vertexKernelSource, const std::string &fragmentKernelSource, const std::string &geometryKernelSource)
	{
		uint32_t kernelRendererIDs[3];
		kernelRendererIDs[0] = CreateKernel(GL_VERTEX_SHADER, vertexKernelSource);
		kernelRendererIDs[1] = CreateKernel(GL_FRAGMENT_SHADER, fragmentKernelSource);
		bool geometryShaderPresent = geometryKernelSource.empty() ? false : true;
		if(geometryShaderPresent)
		{
			kernelRendererIDs[2] = CreateKernel(GL_GEOMETRY_SHADER, geometryKernelSource);
			AddKernels(3, kernelRendererIDs);
		}
		else
			AddKernels(2, kernelRendererIDs);

		#if GM_ENABLE_SHADER_VALIDATION
			Validate();
		#endif
	}

	void Shader::Compile(const std::string &computeShaderSource)
	{
		uint32_t kernelRendererID = CreateKernel(GL_COMPUTE_SHADER, computeShaderSource);
		AddKernels(1, &kernelRendererID);

		#if GM_ENABLE_SHADER_VALIDATION
			Validate();
		#endif
	}

	void Shader::AddKernels(const uint8_t kernelCount, const uint32_t *kernelRendererIDs) const
	{
		for(uint8_t i = 0; i < kernelCount; i++)
		{
			glAttachShader(m_RendererID, kernelRendererIDs[i]);
		}

		glLinkProgram(m_RendererID);

		for(uint8_t i = 0; i < kernelCount; i++)
		{
			uint32_t kernelRendererID = kernelRendererIDs[i];
			glDetachShader(m_RendererID, kernelRendererID);
			glDeleteShader(kernelRendererID);
		}
	}

	void Shader::Validate() const
	{
		int isValid;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &isValid);
		if(!isValid)
		{
			GLint logSize = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &logSize);
			GLchar *infoLog = new GLchar[logSize];
			glGetProgramInfoLog(m_RendererID, logSize, nullptr, infoLog);
			GM_LOG_CORE_ERROR("Failed to link shader | Error: %s", infoLog);
			delete[] infoLog;
		}
	}

	std::string Shader::PreprocessKernel(const std::string &source)
	{
		std::string processedSource("#version ");
		processedSource += std::to_string(GM_GLSL_VERSION) + " core\n";

		const std::string preprocessorDefinitions[] = { "PI 3.14159265359f", "SQRT2 1.41421356237f", "EPSILON 0.000001f" };
		for(const auto &i : preprocessorDefinitions)
		{
			processedSource += "#define " + i + "\n";
		}

		processedSource += source;
		return processedSource;
	}

	void Shader::CompileKernel(const uint32_t rendererID, const GLenum type, const std::string &processedSource)
	{
		const char *source = processedSource.c_str();
		glShaderSource(rendererID, 1, &source, nullptr);
		glCompileShader(rendererID);
	}

	uint32_t Shader::CreateKernel(const GLenum type, const std::string &source)
	{
		uint32_t rendererID = glCreateShader(type);
		Shader::CompileKernel(rendererID, type, PreprocessKernel(source));
		#if GM_ENABLE_SHADER_VALIDATION
			ValidateKernel(rendererID, type);
		#endif
		return rendererID;
	}

	void Shader::ValidateKernel(const uint32_t rendererID, const GLenum type)
	{
		int isValid;
		glGetShaderiv(rendererID, GL_COMPILE_STATUS, &isValid);
		if(!isValid)
		{
			GLint logSize = 0;
			glGetShaderiv(rendererID, GL_INFO_LOG_LENGTH, &logSize);
			GLchar *infoLog = new GLchar[logSize];
			glGetShaderInfoLog(rendererID, logSize, nullptr, infoLog);
			switch(type)
			{
			case GL_VERTEX_SHADER:
				GM_LOG_CORE_ERROR("Failed to compile vertex shader | Error: %s", infoLog);
				break;
			case GL_GEOMETRY_SHADER:
				GM_LOG_CORE_ERROR("Failed to compile geometry shader | Error: %s", infoLog);
				break;
			case GL_FRAGMENT_SHADER:
				GM_LOG_CORE_ERROR("Failed to compile fragment shader | Error: %s", infoLog);
				break;
			case GL_COMPUTE_SHADER:
				GM_LOG_CORE_ERROR("Failed to compile compute shader | Error: %s", infoLog);
				break;
			}

			delete[] infoLog;
		}
	}

	GLint Shader::GetUniformLocation(const std::string &name)
	{
		auto iterator = m_UniformLocations.find(name);
		if(iterator == m_UniformLocations.end())
		{
			GLint location = glGetUniformLocation(m_RendererID, name.c_str());
			#if GM_DEBUG_SHADER_UNIFORMS_ENABLED
				GM_ASSERT(location != -1 || name == "debug" || name == "debug2", "Failed to get uniform location | %s is an invalid name", name)
			#else
				GM_ASSERT(location != -1, "Failed to get uniform location | Invalid name")
			#endif
			m_UniformLocations[name] = location;
			return location;
		}
		else
			return iterator->second;
	}
}