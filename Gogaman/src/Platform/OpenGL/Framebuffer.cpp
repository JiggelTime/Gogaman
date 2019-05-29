#include "pch.h"
#include "Framebuffer.h"
#include "Gogaman/Logging/Log.h"

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

	void Framebuffer::AttachColorBuffer(const Texture &texture, const int level, const int attachmentIndex, bool isRenderTarget)
	{
		GM_ASSERT(attachmentIndex <= m_NumColorAttachments && attachmentIndex >= 0, "Failed to attach framebuffer color buffer: invalid attachment index");
		GM_ASSERT(level >= 1 && (texture.levels >= level || texture.levels == 0),   "Failed to attach framebuffer color buffer: invalid level");

		glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0 + attachmentIndex, texture.GetID(), level - 1);

		if(attachmentIndex == m_NumColorAttachments)
		{
			m_NumColorAttachments++;
			if(isRenderTarget)
			{
				m_RenderTargets.emplace_back(GL_COLOR_ATTACHMENT0 + attachmentIndex);
				glNamedFramebufferDrawBuffers(m_ID, m_RenderTargets.size(), m_RenderTargets.data());
			}
		}
	}

	void Framebuffer::AttachColorBuffer(const Renderbuffer &renderbuffer, const int attachmentIndex)
	{
		GM_ASSERT(attachmentIndex <= m_NumColorAttachments && attachmentIndex >= 0, "Failed to attach framebuffer color buffer: invalid attachment index");

		glNamedFramebufferRenderbuffer(m_ID, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_RENDERBUFFER, renderbuffer.GetID());

		if(attachmentIndex == m_NumColorAttachments)
		{
			m_NumColorAttachments++;
			m_RenderTargets.emplace_back(GL_COLOR_ATTACHMENT0 + attachmentIndex);
			glNamedFramebufferDrawBuffers(m_ID, m_RenderTargets.size(), m_RenderTargets.data());
		}
	}

	void Framebuffer::AttachDepthBuffer(const Texture &texture)
	{
		GM_ASSERT(texture.formatImage == GL_DEPTH_COMPONENT, "Failed to attach framebuffer depth buffer: invalid texture image format");

		glNamedFramebufferTexture(m_ID, GL_DEPTH_ATTACHMENT, texture.GetID(), 0);
	}

	void Framebuffer::AttachDepthBuffer(const Renderbuffer &renderbuffer)
	{
		GM_ASSERT(renderbuffer.formatInternal == GL_DEPTH_COMPONENT16 || renderbuffer.formatInternal == GL_DEPTH_COMPONENT24 || renderbuffer.formatInternal == GL_DEPTH_COMPONENT32 || renderbuffer.formatInternal == GL_DEPTH_COMPONENT32F, "Failed to attach framebuffer depth buffer: invalid renderbuffer internal format");

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