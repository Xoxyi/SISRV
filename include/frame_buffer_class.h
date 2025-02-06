#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <vector>
#include <texture_class.h>
#include <map>
#include <camera_class.h>

class FrameBuffer
{
public:
	unsigned int ID;

	std::map<unsigned int,Texture&> textureAttachments;
	std::map<unsigned int, unsigned int> renderBufferAttachments;

	FrameBuffer(Texture& colorBuffer, Texture& depthBuffer, bool stencil = false);
	FrameBuffer(Texture& colorBuffer);
	FrameBuffer();

	void addColorAttchment(Texture& colorBuffer, unsigned int index);
	void addDepthAttahcment(Texture& depthBuffer);
	void addStencilAttachment();
	
};

FrameBuffer::FrameBuffer(Texture& colorBuffer, Texture& depthbuffer, bool stencil) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer.id, 0);
	textureAttachments[GL_COLOR_ATTACHMENT0] = colorBuffer;

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthbuffer.id, 0);
	textureAttachments[GL_DEPTH_ATTACHMENT] = depthbuffer;

	if (stencil) {
		unsigned int rboStencil;
		glGenRenderbuffers(1, &rboStencil);
		glBindRenderbuffer(GL_RENDERBUFFER, rboStencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboStencil);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		renderBufferAttachments[GL_STENCIL_ATTACHMENT] = rboStencil;
	}

}

FrameBuffer::FrameBuffer() {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

FrameBuffer::FrameBuffer(Texture& colorBuffer) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer.id, 0);
	textureAttachments[GL_COLOR_ATTACHMENT0] = colorBuffer;
}

void FrameBuffer::addColorAttchment(Texture& colorBuffer, unsigned int index) {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	unsigned int attachment = GL_COLOR_ATTACHMENT0 + index;
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, colorBuffer.id, 0);
	textureAttachments[attachment] = colorBuffer;
}

void FrameBuffer::addDepthAttahcment(Texture& depthBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer.id, 0);
	textureAttachments[GL_DEPTH_ATTACHMENT] = depthBuffer;
}

void FrameBuffer::addStencilAttachment() {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	unsigned int rboStencil;
	glGenRenderbuffers(1, &rboStencil);
	glBindRenderbuffer(GL_RENDERBUFFER, rboStencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboStencil);
	renderBufferAttachments[GL_STENCIL_ATTACHMENT] = rboStencil;
}

#endif
