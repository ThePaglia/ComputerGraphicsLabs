#include "fbo.h"
#include <cstdint>
#include <labhelper.h>

FboInfo::FboInfo(int numberOfColorBuffers)
	: isComplete(false), framebufferId(0), depthBuffer(0), width(0), height(0)
{
	colorTextureTargets.resize(numberOfColorBuffers, 0);
};

void FboInfo::resize(int w, int h)
{
	width = w;
	height = h;

	///////////////////////////////////////////////////////////////////////
	// if no texture indices yet, allocate
	///////////////////////////////////////////////////////////////////////
	for (auto& colorTextureTarget : colorTextureTargets)
	{
		if (colorTextureTarget == 0)
		{
			glGenTextures(1, &colorTextureTarget);
			glBindTexture(GL_TEXTURE_2D, colorTextureTarget);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
	}

	if (depthBuffer == 0)
	{
		glGenTextures(1, &depthBuffer);
		glBindTexture(GL_TEXTURE_2D, depthBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	///////////////////////////////////////////////////////////////////////
	// Allocate / Resize textures
	///////////////////////////////////////////////////////////////////////
	for (auto& colorTextureTarget : colorTextureTargets)
	{
		glBindTexture(GL_TEXTURE_2D, colorTextureTarget);
		glTexImage2D(GL_TEXTURE_2D, 0, colorTargetType, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}

	glBindTexture(GL_TEXTURE_2D, depthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
		nullptr);

	///////////////////////////////////////////////////////////////////////
	// Bind textures to framebuffer (if not already done)
	///////////////////////////////////////////////////////////////////////
	if (!isComplete)
	{
		///////////////////////////////////////////////////////////////////////
		// Generate and bind framebuffer
		///////////////////////////////////////////////////////////////////////
		glGenFramebuffers(1, &framebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

		// Bind the color textures as color attachments
		for (int i = 0; i < int(colorTextureTargets.size()); i++)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
				colorTextureTargets[i], 0);
		}
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
								 GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
								 GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
		glDrawBuffers(int(colorTextureTargets.size()), attachments);

		// bind the texture as depth attachment (to the currently bound framebuffer)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

		// check if framebuffer is complete
		isComplete = checkFramebufferComplete();
	}

	// bind default framebuffer, just in case.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool FboInfo::checkFramebufferComplete(void)
{
	// Check that our FBO is correctly set up, this can fail if we have
	// incompatible formats in a buffer, or for example if we specify an
	// invalid drawbuffer, among things.
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (status)
		{
		case GL_FRAMEBUFFER_UNDEFINED:
			labhelper::fatal_error("Framebuffer not complete: GL_FRAMEBUFFER_UNDEFINED");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			labhelper::fatal_error("Framebuffer not complete: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			labhelper::fatal_error("Framebuffer not complete: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			labhelper::fatal_error("Framebuffer not complete: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			labhelper::fatal_error("Framebuffer not complete: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			labhelper::fatal_error("Framebuffer not complete: GL_FRAMEBUFFER_UNSUPPORTED");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			labhelper::fatal_error("Framebuffer not complete: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			labhelper::fatal_error("Framebuffer not complete: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
			break;
		default:
			labhelper::fatal_error("Framebuffer not complete: Unknown error");
			break;
		}
	}

	return (status == GL_FRAMEBUFFER_COMPLETE);
}