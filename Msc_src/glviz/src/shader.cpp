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

#include "shader.hpp"

#include <GL/glew.h>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <memory>
#include <iostream>

#include <Eigen/Dense>

using namespace Eigen;

glShader::glShader()
{
}

glShader::~glShader()
{
    glDeleteShader(m_shader_obj);
}

void
glShader::load_from_file(std::string const& filename)
{
    std::ifstream input(filename.c_str());

    if (input.fail())
		throw file_open_error("Could not open file " + filename);

    std::ostringstream output;
    output << input.rdbuf();

    input.close();

    m_source = output.str();
}

void
glShader::load_from_cstr(char const* source_cstr)
{
    m_source = std::string(source_cstr);
}

void
glShader::compile(std::set<std::string> const& define_list)
{
    std::ostringstream configured_source;

    std::istringstream source_ss(m_source);
    std::string line;

    // Directives like "#version", "#extension" must occur in the shader
    // before anything else, except for comments and white space. Thus
    // insert all defines after the aforementioned directives.
    while (std::getline(source_ss, line))
    {
		std::string token;
		std::istringstream line_ss(line);

		line_ss >> token;

		// Fails for multi row comments.
		if (token == "#version"
			|| token == "#extension"
			|| token == "#pragma"
			|| token.length() == 0)
		{
			configured_source << line << std::endl;
		}
		else
		{
			break;
		}
    }

    for (std::set<std::string>::const_iterator it = define_list.begin();
	    it != define_list.end(); ++it)
    {
		configured_source << "#define " << *it << std::endl;
    }

    configured_source << line << std::endl;
    configured_source << source_ss.rdbuf();

    // Compile configured source.
    const std::string& configured_source_str = configured_source.str();
    const char* configured_source_cstr = configured_source_str.c_str();

    glShaderSource(m_shader_obj, 1, &configured_source_cstr, NULL);
    glCompileShader(m_shader_obj);

    if (!is_compiled())
		throw shader_compilation_error(infolog()); 
}

bool
glShader::is_compiled() const
{
    GLint status;
    glGetShaderiv(m_shader_obj, GL_COMPILE_STATUS, &status);

    return (status == GL_TRUE);
}

std::string
glShader::infolog()
{
    GLint infoLogLength = 0;
    glGetShaderiv(m_shader_obj, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLsizei logLength;

    std::unique_ptr<GLchar> infoLog(new GLchar[infoLogLength]);
    glGetShaderInfoLog(m_shader_obj, infoLogLength, &logLength, infoLog.get());

    return std::string(infoLog.get());
}
glComputeShader::glComputeShader()
{
	m_shader_obj = glCreateShader(GL_COMPUTE_SHADER);
}

glVertexShader::glVertexShader()
{
    m_shader_obj = glCreateShader(GL_VERTEX_SHADER);
}

glFragmentShader::glFragmentShader()
{
    m_shader_obj = glCreateShader(GL_FRAGMENT_SHADER);
}

glGeometryShader::glGeometryShader()
{
    m_shader_obj = glCreateShader(GL_GEOMETRY_SHADER);
}
//extern float voxelConfiguration[1];
glProgram::glProgram()
   // : m_program_obj(glCreateProgram())
{
	m_program_obj = glCreateProgram();
}

glProgram::~glProgram()
{
    glDeleteProgram(m_program_obj);
}

void
glProgram::use() const
{
    glUseProgram(m_program_obj);
}

void
glProgram::unuse() const
{
    glUseProgram(0);
}

void
glProgram::link()
{

    glLinkProgram(m_program_obj);

    if (!is_linked())
		throw shader_link_error(infolog());
}

void
glProgram::attach_shader(glShader& shader)
{
    glAttachShader(m_program_obj, shader.m_shader_obj);
}

void
glProgram::detach_shader(glShader& shader)
{
    glDetachShader(m_program_obj, shader.m_shader_obj);
}

bool
glProgram::is_linked()
{
    GLint status;
    glGetProgramiv(m_program_obj, GL_LINK_STATUS, &status);

    return (status == GL_TRUE);
}

bool
glProgram::is_attached(glShader const& shader)
{
    GLint number_shader_attached;
    glGetProgramiv(m_program_obj, GL_ATTACHED_SHADERS,
	    &number_shader_attached);

    std::unique_ptr<GLuint> shader_list(
	    new GLuint[number_shader_attached]);

    GLsizei count;
    glGetAttachedShaders(m_program_obj,
	    static_cast<GLsizei>(number_shader_attached), &count,
	    shader_list.get());

    for (unsigned int i = 0; i < static_cast<GLuint>(count); ++i)
	if ((shader_list.get())[i] == shader.m_shader_obj)
	    return true;

    return false;
}

std::string
glProgram::infolog()
{
    GLint infoLogLength = 0;
    glGetProgramiv(m_program_obj, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLsizei logLength;

    std::unique_ptr<GLchar> infoLog(new GLchar[infoLogLength]);
    glGetProgramInfoLog(m_program_obj, infoLogLength, &logLength,
        infoLog.get());

    return std::string(infoLog.get());
}

void
//glProgram::set_uniform_block_binding(GLchar const* name, GLuint block_binding)
glProgram::set_uniform_block_binding(GLchar const* name, GLuint block_binding)
{
	GLuint block_index = glGetUniformBlockIndex(m_program_obj, name);
	if (block_index == GL_INVALID_INDEX)
	{
		throw uniform_not_found_error(name);
	}

	glUniformBlockBinding(m_program_obj, block_index, block_binding);
}

void glProgram::setParameter(shaderAttrib type, void* param, char* name)
{
	GLint loc = glGetUniformLocation(m_program_obj, name);

	int val = *((int*)param);
	switch (type)
	{
	case i1:
		glUniform1i(glGetUniformLocation(m_program_obj, name), *((int*)param));
		break;
	case ui1:
		glUniform1ui(glGetUniformLocation(m_program_obj, name), *((unsigned int*)param));
		break;
	case f1:
		glUniform1f(glGetUniformLocation(m_program_obj, name), *((float*)param));
		break;
	case fv3:
		glUniform3fv(glGetUniformLocation(m_program_obj, name), 1, (float*)param);
		break;
	case fv4:
		glUniform4fv(glGetUniformLocation(m_program_obj, name), 1, (float*)param);
		break;
	case mat4x4:
		glUniformMatrix4fv(glGetUniformLocation(m_program_obj, name), 1, GL_FALSE, (float*)param);
		break;
	case mat3x3:
		glUniformMatrix3fv(glGetUniformLocation(m_program_obj, name), 1, GL_FALSE, (float*)param);
		break;
	case img:
		glUniform1i(glGetUniformLocation(m_program_obj, name), *((int*)param));
		break;
	}
}
//void
//glProgram::set_image3D_uniform_binding(GLchar const* name, GLuint binding)
//{
//	
//	//GLint index = glGetUniformLocation(m_program_obj, name);
//	//if (index == GL_INVALID_INDEX)
//	//{
//	//	throw uniform_not_found_error(name);
//	//}
//	glEnable(GL_TEXTURE_3D);
//	
//	GLuint tex;
//	glGenTextures(1, &tex);
//	glBindTexture(GL_TEXTURE_3D, tex);
//
//	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32UI, 100, 100, 101, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);
//	glBindImageTexture(binding, tex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);
//
//	//glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, 100, 100, 101, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
//	//glBindImageTexture(binding, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
//	glMemoryBarrier(GL_ALL_BARRIER_BITS);
//	glBindTexture(GL_TEXTURE_3D, 0);
//}

void
glProgram::set_image2D_uniform_binding(GLchar const* name, GLuint binding)
{

	GLint index = glGetUniformLocation(m_program_obj, name);
	if (index == GL_INVALID_INDEX)
	{
		throw uniform_not_found_error(name);
	}
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 10, 10, 0, GL_RGBA, GL_FLOAT, 0);
	glBindImageTexture(binding, tex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void
glProgram::set_imageBuffer_uniform_binding(GLchar const* name, GLuint binding)
{

	//TODO CHANGE BUFFER AND TEXTURE GENERATION TO SOME OTHER PLACE
	GLuint m_uniform_image_buffer_obj;
	glGenBuffers(1, &m_uniform_image_buffer_obj);


	glBindBuffer(GL_TEXTURE_BUFFER, m_uniform_image_buffer_obj);


	
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_BUFFER, tex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, m_uniform_image_buffer_obj);
	if (glGetError() != GL_NO_ERROR){
		std::cout << "OPENGL ERROR \n";
	}
	glBindImageTexture(2, tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR){
		std::cout << "OPENGL ERROR" << glewGetErrorString(err) << "\n"  ;
		
	}

	//GLuint* count = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	//GLuint block_index = glGetUniformBlockIndex(m_program_obj, name);
	//if (block_index == GL_INVALID_INDEX)
	//{
	//	throw uniform_not_found_error(name);
	//}

	//glUniformBlockBinding(m_program_obj, block_index, 6);
}

GLint
glProgram::get_uniform_Location(GLchar const* name)
{
	GLint index = glGetUniformLocation(m_program_obj, name);
	if (index == GL_INVALID_INDEX)
	{
		throw uniform_not_found_error(name);
	}
	glUniform1i(index, 0);
	return index;
}



