#pragma once

#include "Gogaman/RenderHardwareInterface/Shader.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		struct Texture;
		struct Buffer;

		class Stage;

		class StageBuilder
		{
		public:
			StageBuilder(Stage *stage)
				: m_Stage(stage)
			{}

			~StageBuilder() = default;

			void CreateTexture(const Texture &texture);
			void ReadTexture(const std::string &name);
			void WriteTexture(const std::string &inputName, const std::string &outputName);

			void CreateBuffer(const Buffer &buffer);
			void ReadBuffer(const std::string &name);
			void WriteBuffer(const std::string &inputName, const std::string &outputName);
		private:
			Stage *m_Stage;
		};
	}
}