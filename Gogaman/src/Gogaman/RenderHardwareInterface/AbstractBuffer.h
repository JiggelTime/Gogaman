#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractBuffer : public CRTP<ImplementationType, AbstractBuffer>
		{
		public:
			enum class BindFlags : uint8_t
			{
				Vertex          = 1 << 0,
				Index           = 1 << 1,
				Indirect        = 1 << 2,
				UnorderedAccess = 1 << 3,
				ShaderResource  = 1 << 4,
				ShaderConstants = 1 << 5
			};
		public:
			AbstractBuffer(const AbstractBuffer &) = delete;
			AbstractBuffer(AbstractBuffer &&)      = default;

			AbstractBuffer &operator=(const AbstractBuffer &) = delete;
			AbstractBuffer &operator=(AbstractBuffer &&)      = default;

			inline void SetData(const void *data, const uint32_t size, const uint32_t offset = 0) { return this->GetImplementation().SetData(data, size, offset); }

			inline constexpr uint32_t GetSize() const { return m_Size; }

			inline constexpr BindFlags GetBindFlags() const { return m_BindFlags; }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr       auto &GetNativeData()       { return this->GetImplementation().GetNativeData(); }

			static inline constexpr auto GetNativeBindFlags(const BindFlags bindFlags) { return this->GetImplementation().GetNativeBindFlags(bindFlags); }
		private:
			AbstractBuffer(const uint32_t size, const BindFlags bindFlags)
				: m_Size(size), m_BindFlags(bindFlags)
			{}

			~AbstractBuffer() = default;
		protected:
			uint32_t  m_Size;
			BindFlags m_BindFlags;
		private:
			friend ImplementationType;
		};
	}
}