#pragma once

#include "Resource.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		class StageBuilder;

		class Compiler;

		class Stage
		{
		public:
			using Index = uint8_t;

			using InitializeFunction = std::function<void(StageBuilder &&)>;
			using ExecuteFunction    = std::function<void()>;
		public:
			Stage(const Index index, const InitializeFunction &initialize, const ExecuteFunction &execute)
				: m_Index(index), m_Initialize(initialize), m_Execute(execute)
			{}

			~Stage() = default;

			inline void Initialize(StageBuilder &&builder) { m_Initialize(std::move(builder)); }

			inline void Execute() { m_Execute(); }

			inline constexpr Index GetIndex() const { return m_Index; }
		private:
			std::vector<Texture>     m_CreateTextures;
			std::vector<std::string> m_ReadTextureNames;
			std::vector<std::string> m_WriteTextureInputNames;
			std::vector<std::string> m_WriteTextureOutputNames;

			std::vector<Buffer>      m_CreateBuffers;
			std::vector<std::string> m_ReadBufferNames;
			std::vector<std::string> m_WriteBufferInputNames;
			std::vector<std::string> m_WriteBufferOutputNames;

			InitializeFunction m_Initialize;

			ExecuteFunction m_Execute;

			Index m_Index;
		private:
			friend StageBuilder;

			friend Compiler;
		};
	}
}