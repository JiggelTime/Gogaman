#pragma once

#include "Gogaman/RenderHardwareInterface/Texture.h"

#include "Resource.h"

#include "Gogaman/RenderhardwareInterface/VertexLayout.h"
#include "Gogaman/RenderHardwareInterface/DescriptorGroupLayout.h"
#include "Gogaman/RenderhardwareInterface/ComputeState.h"
#include "Gogaman/RenderhardwareInterface/RenderState.h"

namespace Gogaman
{
	namespace RHI
	{
		class ComputeCommandRecorder;
		class RenderCommandRecorder;
	}

	namespace RenderGraph
	{
		class Graph;

		class ResourceManager;

		class ExecutableGraph;

		class Compiler;

		class Stage
		{
		public:
			using ExecuteFunction = std::function<void(const ResourceManager &)>;

			enum class Type : uint8_t
			{
				Compute,
				PrerecordedCompute,
				Render,
				PrerecordedRender
			};
		private:
			//TODO: Split this up into seperate vectors for better cache behaviour (SOA vs AOS)
			struct CreateTextureData
			{
				Texture             texture;
				std::string         name;
				RHI::Texture::State state;
			};

			struct ReadTextureData
			{
				std::string         name;
				RHI::Texture::State state;
			};

			struct WriteTextureData
			{
				std::string         inputName, outputName;
				RHI::Texture::State state;
			};

			struct CreateBufferData
			{
				Buffer      buffer;
				std::string name;
			};
		public:
			void CreateTexture(const std::string &name, const Texture &texture, const RHI::Texture::State state);
			void ReadTexture(const std::string &name, const RHI::Texture::State state);
			void WriteTexture(const std::string &inputName, const std::string &outputName, const RHI::Texture::State state);

			void CreateBuffer(const std::string &name, const Buffer &buffer);
			void ReadBuffer(const std::string &name);
			void WriteBuffer(const std::string &inputName, const std::string &outputName);

			inline void SetExecuteFunction(ExecuteFunction &&execute) { m_Execute = std::move(execute); }

			//TEMPORARY
			inline constexpr const std::vector<std::string> &GetStateUpdateTextureNames() const { return m_StateUpdateTextureNames; }
		protected:
			Stage()         = default;
			Stage(Stage &&) = default;

			~Stage() = default;
			
			Stage &operator=(Stage &&) = default;
		protected:
			std::vector<CreateTextureData> m_CreateTextureData;
			std::vector<ReadTextureData>   m_ReadTextureData;
			std::vector<WriteTextureData>  m_WriteTextureData;

			std::vector<CreateBufferData> m_CreateBufferData;
			std::vector<std::string>      m_ReadBufferNames;
			std::vector<std::string>      m_WriteBufferInputNames;
			std::vector<std::string>      m_WriteBufferOutputNames;

			std::vector<std::string> m_StateUpdateTextureNames;

			ExecuteFunction m_Execute;
		private:
			friend ExecutableGraph;

			friend Compiler;
		};

		class ComputeStage : public Stage
		{
		public:
			struct StateData
			{
				StateData()             = default;
				StateData(StateData &&) = default;

				~StateData() = default;

				StateData &operator=(StateData &&) = default;

				std::vector<RHI::DescriptorGroupLayout> descriptorGroupLayouts;
				RHI::ShaderProgramID                    shaderProgramID;
			};
		public:
			ComputeStage(StateData &&state)
				: m_StateData(std::move(state))
			{}

			ComputeStage(ComputeStage &&) = default;
			
			~ComputeStage() = default;

			ComputeStage &operator=(ComputeStage &&) = default;
		protected:
			StateData m_StateData;
		private:
			friend Graph;

			friend ExecutableGraph;

			friend Compiler;
		};

		class PrerecordedComputeStage : public ComputeStage
		{
		public:
			using RecordCommandsFunction = std::function<void(RHI::ComputeCommandRecorder &, const StateData &)>;
		public:
			PrerecordedComputeStage(StateData &&state)
				: ComputeStage(std::move(state))
			{}

			PrerecordedComputeStage(PrerecordedComputeStage &&) = default;
			
			~PrerecordedComputeStage() = default;

			PrerecordedComputeStage &operator=(PrerecordedComputeStage &&) = default;

			inline void SetRecordCommandsFunction(RecordCommandsFunction &&recordCommands) { m_RecordCommands = std::move(recordCommands); }
		private:
			RecordCommandsFunction m_RecordCommands;
		private:
			friend Graph;

			friend ExecutableGraph;

			friend Compiler;
		};

		class RenderStage : public Stage
		{
		public:
			struct StateData
			{
				StateData()             = default;
				StateData(StateData &&) = default;

				~StateData() = default;

				StateData &operator=(StateData &&) = default;

				std::vector<RHI::DescriptorGroupLayout> descriptorGroupLayouts;
				std::unique_ptr<RHI::VertexLayout>      vertexLayout;
				RHI::ShaderProgramID                    shaderProgramID;
				RHI::RenderState::DepthStencilState     depthStencilState;
				RHI::RenderState::BlendState            blendState;
				uint16_t                                viewportWidth, viewportHeight;
				RHI::RenderState::CullOperation         cullState = RHI::RenderState::CullOperation::None;
			};
		public:
			RenderStage(StateData &&state)
				: m_StateData(std::move(state))
			{}

			RenderStage(RenderStage &&) = default;
			
			~RenderStage() = default;

			RenderStage &operator=(RenderStage &&) = default;
		protected:
			std::vector<std::string> m_RenderSurfaceAttachmentNames;

			StateData m_StateData;
		private:
			friend Graph;

			friend ExecutableGraph;

			friend Compiler;
		};

		class PrerecordedRenderStage : public RenderStage
		{
		public:
			using RecordCommandsFunction = std::function<void(RHI::RenderCommandRecorder &, const StateData &)>;
		public:
			PrerecordedRenderStage(StateData &&state)
				: RenderStage(std::move(state))
			{}

			PrerecordedRenderStage(PrerecordedRenderStage &&) = default;

			~PrerecordedRenderStage() = default;
			
			PrerecordedRenderStage &operator=(PrerecordedRenderStage &&) = default;

			inline void SetRecordCommandsFunction(RecordCommandsFunction &&recordCommands) { m_RecordCommands = std::move(recordCommands); }
		private:
			RecordCommandsFunction m_RecordCommands;
		private:
			friend Graph;

			friend ExecutableGraph;

			friend Compiler;
		};
	}
}