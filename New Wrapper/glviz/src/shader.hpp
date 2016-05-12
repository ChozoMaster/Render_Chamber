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

#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <string>
#include <set>
#include <map>
#include <stdexcept>

enum shaderAttrib{
	i1, ui1, fv3, fv4, f1, mat4x4, mat3x3, tex, img
};

struct file_open_error : public std::runtime_error
{
    file_open_error(const std::string& errmsg)
		: runtime_error(errmsg)
    {
    }
};

struct shader_compilation_error : public std::logic_error
{
    shader_compilation_error(const std::string& errmsg)
		: logic_error(errmsg)
    {
    }
};

struct shader_link_error : public std::logic_error
{
    shader_link_error(std::string const& errmsg)
		: logic_error(errmsg)
    {
    }
};

struct uniform_not_found_error : public std::logic_error
{
    uniform_not_found_error(std::string const& errmsg)
		: logic_error(errmsg)
    {
    }
};

class glShader
{

public:
	virtual ~glShader();

	void load_from_file(std::string const& filename);
	void load_from_cstr(char const* source_cstr);

	void compile(std::set<std::string> const& define_list =
		std::set<std::string>());
	bool is_compiled() const;

	std::string infolog();

protected:
	glShader();

protected:
	GLuint m_shader_obj;
	std::string m_source;

    friend class glProgram;
};

class glComputeShader : public glShader
{

public:
	glComputeShader();
};

class glVertexShader : public glShader
{

public:
	glVertexShader();
};

class glFragmentShader : public glShader
{

public:
	glFragmentShader();
};

class glGeometryShader : public glShader
{

public:
	glGeometryShader();
};

class glProgram
{

public:
	glProgram();
	virtual ~glProgram();

	void use() const;
	void unuse() const;
	void link();
	
	void attach_shader(glShader& shader);
	void detach_shader(glShader& shader);

	bool is_linked();
	bool is_attached(glShader const& shader);
	std::string infolog();

	void set_uniform_block_binding(GLchar const* name, GLuint block_binding);
	void set_image3D_uniform_binding(GLchar const* name, GLuint binding);
	void set_image2D_uniform_binding(GLchar const* name, GLuint binding);
	GLint get_uniform_Location(GLchar const* name);
	void set_imageBuffer_uniform_binding(GLchar const* name, GLuint binding);
	void setParameter(shaderAttrib type, void* param, char* name);

	GLuint m_program_obj;

protected:
	
};

#endif // SHADER_HPP
