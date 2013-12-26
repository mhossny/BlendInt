/*
 * This file is part of BlendInt (a Blender-like Interface Library in
 * OpenGL).
 *
 * BlendInt (a Blender-like Interface Library in OpenGL) is free
 * software: you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * BlendInt (a Blender-like Interface Library in OpenGL) is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General
 * Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BlendInt.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Contributor(s): Freeman Zhang <zhanggyb@gmail.com>
 */

#include <GL/glew.h>

#include <stdio.h>
#include <stdlib.h>

#include <BlendInt/GLSLShader.hpp>

namespace BlendInt {

	GLSLShader::GLSLShader ()
			: m_id(0)
	{
	}

	GLSLShader::~GLSLShader ()
	{
		Clear();
	}

	GLenum GLSLShader::type () const
	{
		GLint type;
		glGetShaderiv(m_id, GL_SHADER_TYPE, &type);

		return type;
	}

	bool GLSLShader::IsValid () const
	{
		return glIsShader(m_id);
	}

	bool GLSLShader::IsDeleted () const
	{
		GLint status;
		glGetShaderiv(m_id, GL_DELETE_STATUS, &status);

		return status == GL_TRUE ? true : false;
	}

	void GLSLShader::Load (const std::string& filename, GLenum type)
	{
		Clear();

		char* buf = Read(filename.c_str());

		if (buf) {
			m_id = Compile(buf, type);
			free(buf);
		}
	}

	void GLSLShader::Load (const char* buf, GLenum type)
	{
		Clear();

		if (buf) {
			m_id = Compile(buf, type);
		}
	}

	void GLSLShader::Clear ()
	{
		if (m_id) {
			if (glIsShader(m_id)) {
				glDeleteShader(m_id);
			}
			m_id = 0;
		}
	}

	void GLSLShader::PrintLog ()
	{
		GLint log_length = 0;
		if (glIsShader(m_id))
			glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &log_length);
		else {
			fprintf(stderr, "printlog: Not a shader\n");
			return;
		}

		char* log = (char*) malloc(log_length);

		glGetShaderInfoLog(m_id, log_length, NULL, log);

		fprintf(stderr, "%s", log);
		free(log);
	}

	char* GLSLShader::Read (const char* filename)
	{
		FILE * file;
		char * buffer;
		size_t size;

		file = fopen(filename, "rb");
		if (!file) {
			fprintf( stderr, "Unable to open file \"%s\".\n", filename);
			return NULL;
		}
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fseek(file, 0, SEEK_SET);
		buffer = (char *) malloc((size + 1) * sizeof(char *));
		fread(buffer, sizeof(char), size, file);
		buffer[size] = '\0';
		fclose(file);
		return buffer;
	}

	GLuint GLSLShader::Compile (const char* source, const GLenum type)
	{
		GLint compile_status;
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, 0);
		glCompileShader(shader);

		glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
		if (compile_status == GL_FALSE) {
			GLchar messages[256];
			glGetShaderInfoLog(shader, sizeof(messages), 0, &messages[0]);
			fprintf( stderr, "%s\n", messages);
			exit( EXIT_FAILURE);
		}
		return shader;
	}

}
