#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractShader.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class Shader : public AbstractShader<Shader>
		{
		private:
			struct NativeData
			{
				VkShaderModule vulkanShaderModule;
			};
		public:
			Shader(const uint32_t sourceDataSize, const uint8_t *sourceData);
			Shader(const Shader &) = delete;
			Shader(Shader &&)      = default;

			~Shader();

			Shader &operator=(const Shader &) = delete;
			Shader &operator=(Shader &&)      = default;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }

			static inline constexpr VkShaderStageFlagBits GetNativeStage(const Stage stage)
			{
				switch(stage)
				{
				case Stage::Compute:
					return VK_SHADER_STAGE_COMPUTE_BIT;
				case Stage::Vertex:
					return VK_SHADER_STAGE_VERTEX_BIT;
				case Stage::Pixel:
					return VK_SHADER_STAGE_FRAGMENT_BIT;
				default:
					GM_DEBUG_ASSERT(false, "Failed to get native shader stage | Invalid stage");
				}
			}

			static inline constexpr VkShaderStageFlags GetNativeStageFlags(const StageFlag stageFlag)
			{
				VkShaderStageFlags nativeStageFlags = 0;
				if((uint8_t)stageFlag & (uint8_t)StageFlag::Compute)
					nativeStageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
				if((uint8_t)stageFlag & (uint8_t)StageFlag::Vertex)
					nativeStageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
				if((uint8_t)stageFlag & (uint8_t)StageFlag::Pixel)
					nativeStageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;

				return nativeStageFlags;
			}

			static inline constexpr VkFormat GetNativeDataType(const DataType type)
			{
				switch(type)
				{
				case DataType::Bool:
					return VK_FORMAT_R8_UINT;
				case DataType::Int:
					return VK_FORMAT_R32_SINT;
				case DataType::Int2:
					return VK_FORMAT_R32G32_SINT;
				case DataType::Int3:
					return VK_FORMAT_R32G32B32_SINT;
				case DataType::Int4:
					return VK_FORMAT_R32G32B32A32_SINT;
				case DataType::Float:
					return VK_FORMAT_R32_SFLOAT;
				case DataType::Float2:
					return VK_FORMAT_R32G32_SFLOAT;
				case DataType::Float3:
					return VK_FORMAT_R32G32B32_SFLOAT;
				case DataType::Float4:
					return VK_FORMAT_R32G32B32A32_SFLOAT;
				case DataType::Float2x2:
				case DataType::Float3x3:
				case DataType::Float4x4:
					GM_DEBUG_ASSERT(false, "Failed to get native data type | Unsupported data type");
				default:
					GM_DEBUG_ASSERT(false, "Failed to get native data type | Invalid data type");
				}
			}
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractShader;
		};
	}
}