// This file is part of GLviz.
//
// Copyright (C) 2014 by Sebastian Lipponer.
// 
// GLviz is free software: you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// GLviz is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with GLviz. If not, see <http://www.gnu.org/licenses/>.

#include "buffer.hpp"

#include <cassert>
#include <iostream>
#include <vector>
#include "GL\freeglut_std.h"

namespace GLviz
{

glUniformBuffer::glUniformBuffer()
{
	glGenBuffers(1, &m_uniform_buffer_obj);
}

glUniformBuffer::glUniformBuffer(GLsizeiptr size)
	: glUniformBuffer()
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_buffer_obj);
	glBufferData(GL_UNIFORM_BUFFER, size,
		reinterpret_cast<GLfloat*>(0), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glUniformBuffer::~glUniformBuffer()
{
	glDeleteBuffers(1, &m_uniform_buffer_obj);
}

void
glUniformBuffer::bind_buffer_base(GLuint index)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, index, m_uniform_buffer_obj);
}

void
glUniformBuffer::bind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_buffer_obj);
}

void
glUniformBuffer::unbind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glVertexArray::glVertexArray()
{
	glGenVertexArrays(1, &m_vertex_array_obj);
}

glVertexArray::~glVertexArray()
{
	glDeleteBuffers(1, &m_vertex_array_obj);
}

void
glVertexArray::bind()
{
	glBindVertexArray(m_vertex_array_obj);
}

void
glVertexArray::unbind()
{
	glBindVertexArray(0);
}

glArrayBuffer::glArrayBuffer()
{
	glGenBuffers(1, &m_array_buffer_obj);
}

glArrayBuffer::~glArrayBuffer()
{
	glDeleteBuffers(1, &m_array_buffer_obj);
}

void
glArrayBuffer::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_array_buffer_obj);
}

void
glArrayBuffer::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
glArrayBuffer::set_buffer_data(GLsizeiptr size, GLvoid const* ptr)
{
    bind();
   /* glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STREAM_DRAW);*/
   glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, ptr);

    unbind();
}

glElementArrayBuffer::glElementArrayBuffer()
{
	glGenBuffers(1, &m_element_array_buffer_obj);
}

glElementArrayBuffer::~glElementArrayBuffer()
{
	glDeleteBuffers(1, &m_element_array_buffer_obj);
}

void
glElementArrayBuffer::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_array_buffer_obj);
}

void
glElementArrayBuffer::unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
glElementArrayBuffer::set_buffer_data(GLsizeiptr size, GLvoid const* ptr)
{
    bind();

   // glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_STREAM_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, ptr);

    unbind();
}

glAtomicCounter::glAtomicCounter()
{
	glGenBuffers(1, &m_atomicCounter_buffer);
}

glAtomicCounter::~glAtomicCounter()
{
	glDeleteBuffers(1, &m_atomicCounter_buffer);
}

void
glAtomicCounter::bind()
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_atomicCounter_buffer);
}

void
glAtomicCounter::unbind()
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

GLuint
glAtomicCounter::getCount()
{
	GLuint count[1];
	bind();
	glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), count);
	unbind();
	
	return count[0];
}


void
glAtomicCounter::initialize()
{
	glGenBuffers(1, &m_atomicCounter_buffer);
	//glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

	bind();
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, m_atomicCounter_buffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	unbind();
}

void
glAtomicCounter::reset()
{
	GLuint k[1] = { 0 };
	bind();
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), k, GL_DYNAMIC_DRAW);
	unbind();
}

glImageBufferUniform::glImageBufferUniform()
{
	//glGenBuffers(1, &m_uniform_image_buffer_obj);
	//glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, m_uniform_image_buffer_obj);
}


glImageBufferUniform::~glImageBufferUniform()
{
	glDeleteBuffers(1, &m_uniform_image_buffer_obj);
}

void
glImageBufferUniform::initialize(GLuint index, GLuint size, GLenum internalFormat)
{

	if ((m_uniform_image_buffer_obj) > 0)
		glDeleteBuffers(1, &m_uniform_image_buffer_obj);  //delete previously created tbo

	glGenBuffers(1, &m_uniform_image_buffer_obj);
	glBindBuffer(GL_TEXTURE_BUFFER, m_uniform_image_buffer_obj);
	glBufferData(GL_TEXTURE_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	//glClearBufferData(GL_TEXTURE_BUFFER, internalFormat, GL_RED, GL_UNSIGNED_INT, 0);
	if ((tex) > 0)
		glDeleteTextures(1, &tex); //delete previously created texture

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_BUFFER, tex);
	//ptr = glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_WRITE);
	glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, m_uniform_image_buffer_obj);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

}

void
glImageBufferUniform::mapBuffer()
{
	glBindBuffer(GL_TEXTURE_BUFFER, m_uniform_image_buffer_obj);
	ptr = glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_WRITE);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

}

void
glImageBufferUniform::bind()
{
	
	glBindBuffer(GL_TEXTURE_BUFFER, m_uniform_image_buffer_obj);
	glBindTexture(GL_TEXTURE_BUFFER, tex);
	
	
}

void
glImageBufferUniform::unbind()
{
	
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
}

glImage3DUniform::glImage3DUniform()
{

}


glImage3DUniform::~glImage3DUniform()
{
	glDeleteTextures(1, &tex);
}

void
glImage3DUniform::initialize(Eigen::Vector3i size, GLint levels, GLint internalFormat, GLenum format, GLenum type)
{


	
	if ((tex) > 0){
		glDeleteTextures(1, &tex); //delete previously created texture
		
	}

	glEnable(GL_TEXTURE_3D);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_3D, tex);

	/*for (int i = 0; i < levels; i++) {
		glTexImage3D(GL_TEXTURE_3D, i, internalFormat, width, height, depth, 0, format, type, 0);
		glTexSubImage3D(GL_TEXTURE_3D, i, 0, 0, 0, width, height, depth, format, type, 0);
		width = std::max(1, (width / 2));
		height = std::max(1, (height / 2));
		depth = std::max(1, (depth / 2));
	}
*/

	//glTexStorageSparseAMD(GL_TEXTURE_3D, format, size.x(), size.y(), size.z(), levels, GL_TEXTURE_STORAGE_SPARSE_BIT_AMD);
	
	
	//glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, size.x(), size.y(), size.z(), 0, format, type, 0);
	glTexStorage3D(GL_TEXTURE_3D, levels, internalFormat, size.x(), size.y(), size.z());
	glClearTexImage(tex, 0, format, type, 0);

	/*glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, size.x(), size.y(), size.z(), format, type, 0);
	glTexSubImage3D(GL_TEXTURE_3D, 1, 0, 0, 0, size.x(), size.y(), size.z(), format, type, 0);
	glTexSubImage3D(GL_TEXTURE_3D, 2, 0, 0, 0, size.x(), size.y(), size.z(), format, type, 0);
	glTexSubImage3D(GL_TEXTURE_3D, 3, 0, 0, 0, size.x(), size.y(), size.z(), format, type, 0);
	glTexSubImage3D(GL_TEXTURE_3D, 4, 0, 0, 0, size.x(), size.y(), size.z(), format, type, 0);
	glTexSubImage3D(GL_TEXTURE_3D, 5, 0, 0, 0, size.x(), size.y(), size.z(), format, type, 0);
	glTexSubImage3D(GL_TEXTURE_3D, 6, 0, 0, 0, size.x(), size.y(), size.z(), format, type, 0);
	glTexSubImage3D(GL_TEXTURE_3D, 7, 0, 0, 0, size.x(), size.y(), size.z(), format, type, 0);
	glTexSubImage3D(GL_TEXTURE_3D, 8, 0, 0, 0, size.x(), size.y(), size.z(), format, type, 0);
	glTexSubImage3D(GL_TEXTURE_3D, 9, 0, 0, 0, size.x(), size.y(), size.z(), format, type, 0);*/
	//glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);

}

void
glImage3DUniform::bind()
{
	glBindTexture(GL_TEXTURE_3D, tex);
}

void
glImage3DUniform::unbind()
{
	glBindTexture(GL_TEXTURE_3D, 0);
}


glImage2DUniform::glImage2DUniform()
{
	tex = 0;
}


glImage2DUniform::~glImage2DUniform()
{
	glDeleteTextures(1, &tex);
}

void
glImage2DUniform::initialize(Eigen::Vector2i size)
{
	if ((tex) > 0)
		glDeleteTextures(1, &tex); //delete previously created texture

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x(), size.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}



void
glImage2DUniform::bind()
{
	glBindTexture(GL_TEXTURE_2D, tex);
}

void
glImage2DUniform::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}


glFramebufferObject::glFramebufferObject()
{
	m_fbo = 0;
	m_texture = 0;
}

void glFramebufferObject::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void glFramebufferObject::unbind()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void glFramebufferObject::BindForReading()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_texture);

}


void glFramebufferObject::SetReadBuffer()
{
	glReadBuffer(GL_COLOR_ATTACHMENT5);
}

bool glFramebufferObject::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, GLviz::half_screen_width(), GLviz::half_screen_height(), 0, GL_RGB, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, m_texture, 0);
	GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT5 };
	glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);
	// restore default FBO
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
	
}






GBuffer::GBuffer()
{
	m_fbo = 0;
	m_depthTexture = 0;
	ZERO_MEM(m_textures);
}

void GBuffer::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void GBuffer::unbind()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void GBuffer::BindForReading()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
	}
}

void GBuffer::BindDepthTexture(int i)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D,m_depthTexture);
}

void GBuffer::SetReadBuffer(GBUFFER_TEXTURE_TYPE TextureType)
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}

bool GBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight, bool halfRes)
{

	// Create the FBO
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	// Create the gbuffer textures
	glGenTextures(ARRAY_SIZE_IN_ELEMENTS(m_textures), m_textures);
	glGenTextures(1, &m_depthTexture);

	//create position buffer, alpha value is used as valid bit
	glBindTexture(GL_TEXTURE_2D, m_textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WindowWidth, WindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	if (halfRes){
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textures[0], 0);


	for (unsigned int i = 1; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);

		if (halfRes){
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else{
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
	}


	// depth
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
		NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}

}
