#pragma once

namespace Gogaman
{
	namespace RenderGraph
	{
		/*
		enum class ResourceState
		{
			Clear
		};

		struct TextureDescriptor
		{
			uint16_t      width, height;
			TextureFormat format;
			ResourceState initialState;
		};
		*/
		class Stage;
		class TextureResource;
		class Graph;

		class StageBuilder
		{
		public:
			inline StageBuilder(Graph *graph, Stage *stage)
				: m_Graph(graph), m_Stage(stage)
			{}

			inline ~StageBuilder() = default;

			//auto CreateTexture(TextureDescriptor &descriptor);
			//TODO: Template name?
			//TODO: Automatically detect created resource?
			TextureResource &CreateTextureResource(const std::string &name);

			TextureResource &ReadTextureResource(const std::string &name);

			TextureResource &WriteTextureResource(const std::string &name, const std::string &outputName);
		private:
			Graph *m_Graph;
			Stage *m_Stage;
		};
	}
}