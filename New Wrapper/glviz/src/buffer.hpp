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

#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <GL/glew.h>
#include <Eigen/Core>

namespace GLviz
{

class glUniformBuffer
{

public:
	glUniformBuffer();
	glUniformBuffer(GLsizeiptr size);

	~glUniformBuffer();

	void bind_buffer_base(GLuint index);

protected:
	void bind();
	void unbind();

private:
	GLuint m_uniform_buffer_obj;
};

class glVertexArray
{

public:
	glVertexArray();
	~glVertexArray();

	void bind();
	void unbind();

private:
	GLuint m_vertex_array_obj;
};

class glArrayBuffer
{

public:
	glArrayBuffer();
	~glArrayBuffer();

	void bind();
	void unbind();

    void set_buffer_data(GLsizeiptr size, GLvoid const* ptr);

private:
	GLuint m_array_buffer_obj;
};

class glElementArrayBuffer
{

public:
	glElementArrayBuffer();
	~glElementArrayBuffer();

	void bind();
	void unbind();

    void set_buffer_data(GLsizeiptr size, GLvoid const* ptr);

private:
	GLuint m_element_array_buffer_obj;
};

class glAtomicCounter
{
public:
	glAtomicCounter();
	~glAtomicCounter();

	void bind();
	void unbind();
	void initialize();
	void reset();
	GLuint getCount();

private:
	GLuint m_atomicCounter_buffer;
};

class glImageBufferUniform
{
public:
	glImageBufferUniform();
//	glImmageUBufferUniform(GLsizeiptr size);

	~glImageBufferUniform();

	void initialize(GLuint index, GLuint size, GLenum internalFormat);
	GLuint m_uniform_image_buffer_obj, tex;
	void *ptr;
	void bind();
	void unbind();
	void mapBuffer();
protected:


private:
	
};

class glImage3DUniform
{
public:
	glImage3DUniform();
	//	glImmageUBufferUniform(GLsizeiptr size);

	~glImage3DUniform();

	void initialize(Eigen::Vector3i size, GLint levels, GLint internalFormat, GLenum format, GLenum type);
	GLuint tex;
	GLuint clearColor = 0;
	void bind();
	void unbind();
protected:


private:

};

class glImage2DUniform
{
public:
	glImage2DUniform();
	//	glImmageUBufferUniform(GLsizeiptr size);

	~glImage2DUniform();

	void initialize(Eigen::Vector2i size);
	GLuint tex;
	void bind();
	void unbind();
protected:


private:

};


class glFramebufferObject
{
public:

	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_TANGENT,
		GBUFFER_TEXTURE_TYPE_BITANGENT,
		GBUFFER_TEXTURE_TYPE_DIFFUSECOLOR,
		GBUFFER_NUM_TEXTURES
	};

	glFramebufferObject();

	bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

	void BindForWriting();

	void BindForReading();

	void unbind();

	void SetReadBuffer();

private:

	GLuint m_fbo;
	GLuint m_texture;
};


#define GBUFFER_POSITION_TEXTURE_UNIT 0
#define GBUFFER_NORMAL_TEXTURE_UNIT  1
#define GBUFFER_TANGENT_TEXTURE_UNIT   2
#define GBUFFER_BITANGENT_TEXTURE_UNIT 3
#define GBUFFER_COLOR_TEXTURE_UNIT 4
#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))


class GBuffer
{
public:

	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_TANGENT,
		GBUFFER_TEXTURE_TYPE_BITANGENT,
		GBUFFER_TEXTURE_TYPE_DIFFUSECOLOR,
		GBUFFER_NUM_TEXTURES
	};

	GBuffer();

	bool Init(unsigned int WindowWidth, unsigned int WindowHeight, bool halfRes);

	void BindForWriting();

	void BindForReading();

	void unbind();

	void SetReadBuffer(GBUFFER_TEXTURE_TYPE TextureType);
	void BindDepthTexture(int i);

private:

	GLuint m_fbo;
	GLuint m_textures[GBUFFER_NUM_TEXTURES];
	GLuint m_depthTexture;
};



}

#endif // BUFFER_HPP
