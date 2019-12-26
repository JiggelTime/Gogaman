#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Gogaman/Core/Base.h"

#define GM_RHI_SHADER_STAGE_COUNT 3

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractShader : public CRTP<ImplementationType, AbstractShader>
		{
		public:
			enum class Stage : uint8_t
			{
				Compute,
				Vertex,
				Pixel
			};

			enum class StageFlags : uint8_t
			{
				Compute = 1 << 0,
				Vertex  = 1 << 1,
				Pixel   = 1 << 2,
				All     = UINT8_MAX
			};

			enum class DataType : uint8_t
			{
				Bool,
				Int,
				Int2,
				Int3,
				Int4,
				Float,
				Float2,
				Float3,
				Float4,
				Float2x2,
				Float3x3,
				Float4x4
			};
		public:
			AbstractShader(const AbstractShader &) = delete;
			AbstractShader(AbstractShader &&)      = default;

			AbstractShader &operator=(const AbstractShader &) = delete;
			AbstractShader &operator=(AbstractShader &&)      = default;

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr       auto &GetNativeData()       { return this->GetImplementation().GetNativeData(); }

			static inline constexpr auto GetNativeStage(const Stage stage) { return ImplementationType::GetNativeStage(stage); }

			static inline constexpr auto GetNativeStageFlags(const StageFlags stageFlags) { return ImplementationType::GetNativeStageFlags(stageFlags); }

			static inline constexpr uint8_t GetDataTypeSize(const DataType type)
			{
				switch(type)
				{
				case DataType::Bool:
					return sizeof(bool);
				case DataType::Int:
					return sizeof(int);
				case DataType::Int2:
					return sizeof(int) * 2;
				case DataType::Int3:
					return sizeof(int) * 3;
				case DataType::Int4:
					return sizeof(int) * 4;
				case DataType::Float:
					return sizeof(float);
				case DataType::Float2:
					return sizeof(float) * 2;
				case DataType::Float3:
					return sizeof(float) * 3;
				case DataType::Float4:
					return sizeof(float) * 4;
				case DataType::Float2x2:
					return sizeof(float) * 2 * 2;
				case DataType::Float3x3:
					return sizeof(float) * 3 * 3;
				case DataType::Float4x4:
					return sizeof(float) * 4 * 4;
				default:
					GM_DEBUG_ASSERT(false, "Failed to get data type size | Invalid data type");
				}
			}

			static inline constexpr DataType GetBaseDataType(const DataType type)
			{
				switch(type)
				{
				case DataType::Bool:
					return DataType::Bool;
				case DataType::Int:
				case DataType::Int2:
				case DataType::Int3:
				case DataType::Int4:
					return DataType::Int;
				case DataType::Float:
				case DataType::Float2:
				case DataType::Float3:
				case DataType::Float4:
				case DataType::Float2x2:
				case DataType::Float3x3:
				case DataType::Float4x4:
					return DataType::Float;
				default:
					GM_DEBUG_ASSERT(false, "Failed to get base data type | Invalid data type");
				}
			}

			static inline constexpr uint8_t GetShaderDataTypeComponentCount(const DataType dataType) { return GetDataTypeSize(dataType) / GetDataTypeSize(GetBaseDataType(dataType)); }

			static inline constexpr auto GetNativeDataType(const DataType type) { return ImplementationType::GetNativeDataType(type); }
		private:
			AbstractShader(const uint32_t sourceDataSize, const uint8_t *sourceData)
			{
				GM_DEBUG_ASSERT(sourceDataSize > 0, "Failed to construct shader | Invalid source data size");
			}

			~AbstractShader() = default;
		private:
			friend ImplementationType;
		};
	}
}