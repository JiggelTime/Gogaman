#include "pch.h"
#include "Framebuffer.h"

namespace Gogaman
{
	Framebuffer::Framebuffer()
		: m_ID(0), m_NumColorAttachments(0)
	{
		glCreateFramebuffers(1, &m_ID);
	}

	Framebuffer::~Framebuffer()
	{
		if(!m_ID)
			return;

		glDeleteFramebuffers(1, &m_ID);
	}

	void Framebuffer::AttachColorBuffer(const Texture2D &texture, const int level, const int attachmentIndex, bool renderTarget)
	{
		GM_ASSERT(attachmentIndex <= m_NumColorAttachments && attachmentIndex >= 0);
		GM_ASSERT(level >= 1);
		GM_ASSERT(texture.levels >= level || texture.levels == 0);

		glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0 + attachmentIndex, texture.GetID(), level - 1);

		if(attachmentIndex == m_NumColorAttachments)
		{
			m_NumColorAttachments++;
			if(renderTarget)
			{
				m_RenderTargets.emplace_back(GL_COLOR_ATTACHMENT0 + attachmentIndex);
				glNamedFramebufferDrawBuffers(m_ID, m_RenderTargets.size(), m_RenderTargets.data());
			}
		}
	}

	void Framebuffer::AttachColorBuffer(const Renderbuffer &renderbuffer, const int attachmentIndex)
	{
		GM_ASSERT(attachmentIndex <= m_NumColorAttachments && attachmentIndex >= 0);

		glNamedFramebufferRenderbuffer(m_ID, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_RENDERBUFFER, renderbuffer.GetID());

		if(attachmentIndex == m_NumColorAttachments)
		{
			m_NumColorAttachments++;
			m_RenderTargets.emplace_back(GL_COLOR_ATTACHMENT0 + attachmentIndex);
			glNamedFramebufferDrawBuffers(m_ID, m_RenderTargets.size(), m_RenderTargets.data());
		}
	}

	void Framebuffer::AttachDepthBuffer(const Texture2D &texture)
	{
		GM_ASSERT(texture.formatImage == GL_DEPTH_COMPONENT);

		glNamedFramebufferTexture(m_ID, GL_DEPTH_ATTACHMENT, texture.GetID(), 0);
	}

	void Framebuffer::AttachDepthBuffer(const Renderbuffer &renderbuffer)
	{
		GM_ASSERT(renderbuffer.formatInternal == GL_DEPTH_COMPONENT16 || renderbuffer.formatInternal == GL_DEPTH_COMPONENT24 || renderbuffer.formatInternal == GL_DEPTH_COMPONENT32 || renderbuffer.formatInternal == GL_DEPTH_COMPONENT32F);

		glNamedFramebufferRenderbuffer(m_ID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.GetID());
	}

	void Framebuffer::Clear() const
	{
		const GLfloat clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		for(int i = 0; i < m_NumColorAttachments; i++)
			glClearNamedFramebufferfv(m_ID, GL_COLOR, i, clearColor);

		const GLfloat depthClearColor = 0.0f;
		glClearNamedFramebufferfv(m_ID, GL_DEPTH, 0, &depthClearColor);
	}
}