#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include "cube_map_class.h"
#include "texture_class.h"
#include <map>
#include "camera_class.h"

class FrameBuffer
{
public:
	unsigned int ID;

	std::map<unsigned int,Texture*> textureAttachments;
	std::map<unsigned int, unsigned int> renderBufferAttachments;

	FrameBuffer(Texture *colorBuffer, Texture *depthBuffer, bool stencil = false);
	FrameBuffer(Texture* colorBuffer);
	FrameBuffer();

	void addColorAttchment(Texture *colorBuffer, unsigned int index, int texTarget = GL_TEXTURE_2D);
	void addDepthAttahcment(Texture *depthBuffer);
	void addDephAttachment(int width = SCR_WIDTH, int height = SCR_HEIGHT);
	void addStencilAttachment();
	void updateAttachment(std::vector<unsigned int> indices);
	int checkCompleteness();
	void enable();
	void disable();
	
};

FrameBuffer::FrameBuffer(Texture *colorBuffer, Texture *depthbuffer, bool stencil) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer->id, 0);
	textureAttachments[GL_COLOR_ATTACHMENT0] = colorBuffer;

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthbuffer->id, 0);
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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::FrameBuffer() {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

FrameBuffer::FrameBuffer(Texture *colorBuffer) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer->id, 0);
	textureAttachments[GL_COLOR_ATTACHMENT0] = colorBuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::addColorAttchment(Texture *colorBuffer, unsigned int index, int texTarget) {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	unsigned int attachment = GL_COLOR_ATTACHMENT0 + index;
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texTarget, colorBuffer->id, 0);
	textureAttachments[attachment] = colorBuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


/*void FrameBuffer::addDepthAttahcment(Texture *depthBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer->id, 0);
	textureAttachments[GL_DEPTH_ATTACHMENT] = depthBuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}*/

void FrameBuffer::addDepthAttahcment(Texture *depthBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::addDephAttachment(int width, int height) {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);

    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	renderBufferAttachments[GL_DEPTH_ATTACHMENT] = rboDepth;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::addStencilAttachment() {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	unsigned int rboStencil;
	glGenRenderbuffers(1, &rboStencil);
	glBindRenderbuffer(GL_RENDERBUFFER, rboStencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboStencil);
	renderBufferAttachments[GL_STENCIL_ATTACHMENT] = rboStencil;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::updateAttachment(std::vector<unsigned int> indices) {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	unsigned int *attachments = (unsigned int*)(malloc(indices.size() * sizeof(unsigned int)));
	for(unsigned int i = 0; i < indices.size(); i++) {
		attachments[i] = GL_COLOR_ATTACHMENT0 + indices[i];
	}
	glDrawBuffers(indices.size(), attachments);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int FrameBuffer::checkCompleteness()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return -1;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return 1;
}

void FrameBuffer::enable()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void FrameBuffer::disable()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#endif
