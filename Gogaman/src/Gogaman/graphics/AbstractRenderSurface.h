#pragma once

#include "Gogaman/CRTP.h"
#include "Texture.h"
#include "Platform/OpenGL/OpenGL_Renderbuffer.h"

namespace Gogaman
{
	using RenderSurfaceID = uint8_t;

	template<typename RenderSurfaceType>
	class AbstractRenderSurface : public CRTP<RenderSurfaceType, AbstractRenderSurface>
	{
	public:
		inline void UploadData(const uint32_t size, const uint16_t *indices) const { this->GetImplementation().UploadData(size, indices); }

		inline void AttachColorBuffer(const Texture &texture)                                                                  { this->GetImplementation().AttachColorBuffer(texture);                                         }
		inline void AttachColorBuffer(const Texture &texture, bool isRenderTarget)                                             { this->GetImplementation().AttachColorBuffer(texture, isRenderTarget);                         }
		inline void AttachColorBuffer(const Texture &texture, const int level, bool isRenderTarget)                            { this->GetImplementation().AttachColorBuffer(texture, level, isRenderTarget);                  }
		inline void AttachColorBuffer(const Texture &texture, const int level, const int attachmentIndex, bool isRenderTarget) { this->GetImplementation().AttachColorBuffer(texture, level, attachmentIndex, isRenderTarget); }

		inline void AttachColorBuffer(const Renderbuffer &renderbuffer)                            { this->GetImplementation().AttachColorBuffer(renderbuffer);                  }
		inline void AttachColorBuffer(const Renderbuffer &renderbuffer, const int attachmentIndex) { this->GetImplementation().AttachColorBuffer(renderbuffer, attachmentIndex); }
		
		inline void AttachDepthBuffer(const Texture      &texture)      { this->GetImplementation().AttachDepthBuffer(texture);      }
		inline void AttachDepthBuffer(const Renderbuffer &renderbuffer) { this->GetImplementation().AttachDepthBuffer(renderbuffer); }

		inline void Bind()   const { this->GetImplementation().Bind();   }
		inline void Unbind() const { this->GetImplementation().Unbind(); }
		
		inline void Clear()  const { this->GetImplementation().Clear();  }

		inline void BlitColorBuffer(const   RenderSurfaceType &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const { this->GetImplementation().BlitColorBuffer(source,   width, height, interpolationMode); }
		inline void BlitDepthBuffer(const   RenderSurfaceType &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const { this->GetImplementation().BlitDepthBuffer(source,   width, height, interpolationMode); }
		inline void BlitStencilBuffer(const RenderSurfaceType &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const { this->GetImplementation().BlitStencilBuffer(source, width, height, interpolationMode); }

		inline int GetNumColorAttachments() const { return this->GetImplementation().GetNumColorAttachments(); }
		inline int GetNumRenderTargets()    const { return this->GetImplementation().GetNumRenderTargets();    }
	protected:
		~AbstractRenderSurface() = default;
	public:
		RenderSurfaceID identifer;
	};
}