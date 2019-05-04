#pragma once

#include "Gogaman/Core.h"
#include "Texture2D.h"
#include "Renderbuffer.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API Framebuffer
	{
	public:
		Framebuffer(const uint width, const uint height);
		~Framebuffer();

		void AttachColorBuffer(Texture2D &texture) { AttachColorBuffer(texture, 1); }
		void AttachColorBuffer(Texture2D &texture, const uint level) { AttachColorBuffer(texture, level, m_NumColorAttachments); }
		void AttachColorBuffer(Texture2D &texture, const uint level, const uint attachmentIndex);
		void AttachColorBuffer(Renderbuffer &renderbuffer) { AttachColorBuffer(renderbuffer, m_NumColorAttachments); }
		void AttachColorBuffer(Renderbuffer &renderbuffer, const uint attachmentIndex);
		
		void AttachDepthBuffer(Texture2D &texture);
		void AttachDepthBuffer(Renderbuffer &renderbuffer);

		inline void Bind()   const { glBindFramebuffer(GL_FRAMEBUFFER, m_ID); }
		inline void Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
		
		inline void Destroy() const { glDeleteFramebuffers(1, &m_ID); }

		void Clear() const;

		inline uint GetWidth()  const { return m_Width; }
		inline uint GetHeight() const { return m_Height; }

		inline uint GetNumColorAttachments() const { return m_NumColorAttachments; }
	private:
		uint m_ID;
		uint m_Width, m_Height;
		uint m_NumColorAttachments;
	};
}