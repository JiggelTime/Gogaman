#include "pch.h"
#include "Framebuffer.h"

namespace Gogaman
{
	Framebuffer::Framebuffer(const uint width, const uint height)
		: m_Width(width), m_Height(height), m_NumColorAttachments(0)
	{
		glCreateFramebuffers(1, &m_ID);
	}

	Framebuffer::~Framebuffer()
	{}

	void Framebuffer::AttachColorBuffer(Texture2D &texture, const uint level, const uint attachmentIndex)
	{
		GM_ASSERT(attachmentIndex <= m_NumColorAttachments);
		GM_ASSERT(texture.width == m_Width && texture.height == m_Height);
		GM_ASSERT(level >= 1);
		GM_ASSERT(texture.levels >= level || texture.levels == 0);

		glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0 + attachmentIndex, texture.GetID(), level - 1);
		if(attachmentIndex == m_NumColorAttachments)
			m_NumColorAttachments++;
	}

	void Framebuffer::AttachColorBuffer(Renderbuffer &renderbuffer, const uint attachmentIndex)
	{
		GM_ASSERT(attachmentIndex <= m_NumColorAttachments);
		GM_ASSERT(renderbuffer.width == m_Width && renderbuffer.height == m_Height);

		glNamedFramebufferRenderbuffer(m_ID, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_RENDERBUFFER, renderbuffer.GetID());
		if(attachmentIndex == m_NumColorAttachments)
			m_NumColorAttachments++;
	}

	void Framebuffer::AttachDepthBuffer(Texture2D &texture)
	{
		GM_ASSERT(texture.width == m_Width && texture.height == m_Height);
		GM_ASSERT(texture.formatImage == GL_DEPTH_COMPONENT);

		glNamedFramebufferTexture(m_ID, GL_DEPTH_ATTACHMENT, texture.GetID(), 0);
	}

	void Framebuffer::AttachDepthBuffer(Renderbuffer &renderbuffer)
	{
		GM_ASSERT(renderbuffer.width == m_Width && renderbuffer.height == m_Height);
		GM_ASSERT(renderbuffer.formatInternal == GL_DEPTH_COMPONENT16 || renderbuffer.formatInternal == GL_DEPTH_COMPONENT24 || renderbuffer.formatInternal == GL_DEPTH_COMPONENT32 || renderbuffer.formatInternal == GL_DEPTH_COMPONENT32F);

		glNamedFramebufferRenderbuffer(m_ID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.GetID());
	}

	void Framebuffer::Clear() const
	{

	}
}