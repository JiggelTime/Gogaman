#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Texture/Texture.h"
#include "Platform/OpenGL/OpenGL_RenderBuffer.h"

namespace Gogaman
{
	template<typename ImplementationType>
	class AbstractRenderSurface : public CRTP<ImplementationType, AbstractRenderSurface>
	{
	public:
		AbstractRenderSurface(const AbstractRenderSurface &) = delete;
		AbstractRenderSurface(AbstractRenderSurface &&other) = default;

		AbstractRenderSurface &operator=(const AbstractRenderSurface &) = delete;
		AbstractRenderSurface &operator=(AbstractRenderSurface &&other) = default;

		inline void UploadData(const uint32_t size, const uint16_t *indices) const { this->GetImplementation().UploadData(size, indices); }

		inline void AddColorBuffer(const Texture &texture)                                                                        { this->GetImplementation().AddColorBuffer(texture);                                         }
		inline void AddColorBuffer(const Texture &texture, const bool isRenderTarget)                                             { this->GetImplementation().AddColorBuffer(texture, isRenderTarget);                         }
		inline void AddColorBuffer(const Texture &texture, const int level, const bool isRenderTarget)                            { this->GetImplementation().AddColorBuffer(texture, level, isRenderTarget);                  }
		inline void AddColorBuffer(const Texture &texture, const int level, const int attachmentIndex, const bool isRenderTarget) { this->GetImplementation().AddColorBuffer(texture, level, attachmentIndex, isRenderTarget); }

		inline void AddColorBuffer(const RenderBuffer &renderbuffer)                            { this->GetImplementation().AddColorBuffer(renderbuffer);                  }
		inline void AddColorBuffer(const RenderBuffer &renderbuffer, const int attachmentIndex) { this->GetImplementation().AddColorBuffer(renderbuffer, attachmentIndex); }
		
		inline void AddDepthBuffer(const Texture      &texture)      { this->GetImplementation().AddDepthBuffer(texture);      }
		inline void AddDepthBuffer(const RenderBuffer &renderbuffer) { this->GetImplementation().AddDepthBuffer(renderbuffer); }

		inline void Bind()   const { this->GetImplementation().Bind();   }
		inline void Unbind() const { this->GetImplementation().Unbind(); }
		
		inline void Clear()  const { this->GetImplementation().Clear();  }

		inline void CopyColorBuffer(const   ImplementationType &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const { this->GetImplementation().CopyColorBuffer(source,   width, height, interpolationMode); }
		inline void CopyDepthBuffer(const   ImplementationType &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const { this->GetImplementation().CopyDepthBuffer(source,   width, height, interpolationMode); }
		inline void CopyStencilBuffer(const ImplementationType &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const { this->GetImplementation().CopyStencilBuffer(source, width, height, interpolationMode); }

		inline constexpr int GetColorAttachmentCount() const { return this->GetImplementation().GetColorAttachmentCount(); }
		inline           int GetRenderTargetCount()    const { return this->GetImplementation().GetRenderTargetCount();    }

		static inline void BindBackBuffer()  { ImplementationType::BindBackBuffer();  }

		static inline void ClearBackBuffer() { ImplementationType::ClearBackBuffer(); }
	protected:
		AbstractRenderSurface()  = default;
		~AbstractRenderSurface() = default;
	};
}