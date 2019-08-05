#pragma once

#include "Gogaman/CRTP.h"
#include "Gogaman/Resource.h"

#include "Texture/Texture.h"
#include "Platform/OpenGL/OpenGL_Renderbuffer.h"

namespace Gogaman
{
	using RenderSurfaceID = uint8_t;

	template<typename ImplementationType>
	class AbstractRenderSurface : public CRTP<ImplementationType, AbstractRenderSurface>, public Resource<RenderSurfaceID>
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

		inline void AddColorBuffer(const Renderbuffer &renderbuffer)                            { this->GetImplementation().AddColorBuffer(renderbuffer);                  }
		inline void AddColorBuffer(const Renderbuffer &renderbuffer, const int attachmentIndex) { this->GetImplementation().AddColorBuffer(renderbuffer, attachmentIndex); }
		
		inline void AddDepthBuffer(const Texture      &texture)      { this->GetImplementation().AddDepthBuffer(texture);      }
		inline void AddDepthBuffer(const Renderbuffer &renderbuffer) { this->GetImplementation().AddDepthBuffer(renderbuffer); }

		inline void Bind()   const { this->GetImplementation().Bind();   }
		inline void Unbind() const { this->GetImplementation().Unbind(); }
		
		inline void Clear()  const { this->GetImplementation().Clear();  }

		inline void BlitColorBuffer(const   ImplementationType &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const { this->GetImplementation().BlitColorBuffer(source,   width, height, interpolationMode); }
		inline void BlitDepthBuffer(const   ImplementationType &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const { this->GetImplementation().BlitDepthBuffer(source,   width, height, interpolationMode); }
		inline void BlitStencilBuffer(const ImplementationType &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const { this->GetImplementation().BlitStencilBuffer(source, width, height, interpolationMode); }

		inline int GetNumColorAttachments() const { return this->GetImplementation().GetNumColorAttachments(); }
		inline int GetNumRenderTargets()    const { return this->GetImplementation().GetNumRenderTargets();    }

		static inline void BindBackBuffer()  { ImplementationType::BindBackBuffer();  }

		static inline void ClearBackBuffer() { ImplementationType::ClearBackBuffer(); }
	protected:
		AbstractRenderSurface()  = default;
		~AbstractRenderSurface() = default;
	};
}