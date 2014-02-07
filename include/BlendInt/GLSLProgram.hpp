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

#ifndef _BLENDINT_PROGRAM_HPP_
#define _BLENDINT_PROGRAM_HPP_

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <BlendInt/Object.hpp>
#include <BlendInt/GLSLShader.hpp>

namespace BlendInt {

	/**
	 * @brief Wrapper class for OpenGL Programs
	 *
	 * Usage:
	 * 	-# Define or new a GLSLProgram object, e.g. GLSLProgram foo; // or GLSLProgram* foo = new GLSLProgram;
	 * 	-# Call Create(): foo.Create()
	 * 	-# Attach shaders: foo.AttachShaderPair(vs_shader, fs_shader);
	 * 	-# Call Link(): foo.Link()
	 * 	-# Call Activate() for drawing: foo.Activate();	// and draw sth
	 * 	-# Call Deactivate() after drawing: foo.Deactivate();
	 *
	 * @ingroup opengl
	 */
	class GLSLProgram: public Object
	{
	public:

		/**
		 * @brief Default constructor
		 */
		GLSLProgram ();

		/**
		 * @brief Create the GLSL program
		 * @return
		 * 	- true if success
		 * 	- false if fail
		 */
		bool Create ();

		/**
		 * @brief Attach shader
		 * @param[in] buf The shader stored in memory as string
		 * @param[in] type The shader type, must be one of:
		 * 	- GL_VERTEX_SHADER
		 * 	- GL_TESS_CONTROL_SHADER
		 * 	- GL_TESS_EVALUATION_SHADER
		 * 	- GL_GEOMETRY_SHADER
		 * 	- GL_FRAGMENT_SHADER
		 *
		 * The param type is defined in manual of glCreateShader
		 */
		void AttachShader (const char* buf, GLenum type);

		/**
		 * @brief Attach shader
		 * @param[in] filename The path of the shader file
		 * @param[in] type The shader type, must be one of:
		 * 	- GL_VERTEX_SHADER
		 * 	- GL_TESS_CONTROL_SHADER
		 * 	- GL_TESS_EVALUATION_SHADER
		 * 	- GL_GEOMETRY_SHADER
		 * 	- GL_FRAGMENT_SHADER
		 *
		 * The param type is defined in manual of glCreateShader
		 */
		void AttachShader (const std::string& filename, GLenum type);

		void AttachShader (const GLSLShader& shader);

		/**
		 * @brief Attach vertex and fragment shaders
		 * @param vertex_shader The vertex shader stored in memory as string
		 * @param fragment_shader The vertex shader stored in memory as string
		 */
		void AttachShaderPair (const char* vertex_shader,
		        const char* fragment_shader);

		/**
		 * @brief Attach vertex and fragment shaders
		 * @param vertex_shader File name of the vertex shader
		 * @param fragment_shader File name of the fragment shader
		 */
		void AttachShaderPair (const std::string& vertex_shader, const std::string& fragment_shader);

		/**
		 * @brief Link the program
		 * @return
		 * 	- true if success
		 * 	- false if fail
		 *
		 * 	@note this function should be called after attaching correct shaders
		 */
		bool Link ();

		bool DetachShader (GLuint shader);

		bool DetachShader (const GLSLShader& shader);

		/**
		 * @brief Get if the program is valid
		 * @return
		 * 	- true if valid
		 * 	- false if not valid
		 */
		bool IsValid () const;

		/**
		 * @brief Clear all shaders created
		 */
		void Clear ();

		/**
		 * @brief Use this program for render
		 */
		void Activate ();

		/**
		 * @brief Unuse this program for render
		 */
		void Deactivate ();

		/**
		 * @brief Print log
		 */
		void PrintLog ();

		/**
		 * @brief Get the program id
		 * @return GLSL program id
		 */
		GLuint id () const
		{
			return m_id;
		}

		/**
		 * @brief Get attribute location
		 * @param name The attribute name
		 * @return The attribute location
		 */
		GLint GetAttributeLocation (const char *name);

		/**
		 * @brief Get uniform location
		 * @param name The uniform name
		 * @return The uniform location
		 */
		GLint GetUniformLocation (const char *name);

	protected:

		/**
		 * @brief Destructor
		 */
		virtual ~GLSLProgram ();

	private:

		GLuint m_id;
	};

}

#endif /* _BIL_PROGRAM_HPP_ */
