/*
 * This file is part of BlendInt (a Blender-like Interface Library in
 * OpenGL).
 *
 * BlendInt (a Blender-like Interface Library in OpenGL) is free software:
 * you can redistribute it and/or modify it under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * BlendInt (a Blender-like Interface Library in OpenGL) is distributed in
 * the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BlendInt.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Contributor(s): Freeman Zhang <zhanggyb@gmail.com>
 */

#ifdef __UNIX__
#ifdef __APPLE__
#include <gl3.h>
#include <gl3ext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif  // __UNIX__

#include <algorithm>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <BlendInt/Gui/CircularPicker.hpp>
#include <BlendInt/Gui/VertexTool.hpp>
#include <BlendInt/Stock/Theme.hpp>
#include <BlendInt/Stock/Shaders.hpp>

namespace BlendInt {

	CircularPicker::CircularPicker ()
	: AbstractRoundForm(), vao_(0)
	{
		set_round_type(RoundAll);

		glGenVertexArrays(1, &vao_);

		inner_.reset(new GLArrayBuffer);
		inner_->generate();
		outer_.reset(new GLArrayBuffer);
		outer_->generate();
	}

	CircularPicker::~CircularPicker ()
	{
		glDeleteVertexArrays(1, &vao_);
	}

	void CircularPicker::Resize(unsigned int radius)
	{
		Size dot_size (radius * 2, radius * 2);
		set_round_type(RoundAll);
		set_radius(radius);
		set_size(dot_size);

		VertexTool tool;
		tool.GenerateVertices(dot_size, DefaultBorderWidth(), RoundAll, radius);

		inner_->bind();
		inner_->set_data(tool.inner_size(), tool.inner_data());
		outer_->bind();
		outer_->set_data(tool.outer_size(), tool.outer_data());
		GLArrayBuffer::reset();
	}

	void CircularPicker::PerformSizeUpdate(const Size& size)
	{
		VertexTool tool;
		tool.GenerateVertices(size, DefaultBorderWidth(), round_type(), radius());
		inner_->bind();
		inner_->set_data(tool.inner_size(), tool.inner_data());
		outer_->bind();
		outer_->set_data(tool.outer_size(), tool.outer_data());
		GLArrayBuffer::reset();

		set_size(size);
	}

	void CircularPicker::PerformRoundTypeUpdate(int type)
	{
		VertexTool tool;
		tool.GenerateVertices(size(), DefaultBorderWidth(), type, radius());
		inner_->bind();
		inner_->set_data(tool.inner_size(), tool.inner_data());
		outer_->bind();
		outer_->set_data(tool.outer_size(), tool.outer_data());
		GLArrayBuffer::reset();

		set_round_type(type);
	}

	void CircularPicker::PerformRoundRadiusUpdate(float radius)
	{
		VertexTool tool;
		tool.GenerateVertices(size(), DefaultBorderWidth(), round_type(), radius);
		inner_->bind();
		inner_->set_data(tool.inner_size(), tool.inner_data());
		outer_->bind();
		outer_->set_data(tool.outer_size(), tool.outer_data());
		GLArrayBuffer::reset();

		set_radius(radius);
	}

	void CircularPicker::Draw (const glm::vec3& pos, short gamma) const
	{
		using Stock::Shaders;

		glBindVertexArray(vao_);

		RefPtr<GLSLProgram> program =
				Shaders::instance->triangle_program();
		program->use();

		program->SetUniform3fv("u_position", 1, glm::value_ptr(pos));
		program->SetUniform1i("u_gamma", gamma);
		program->SetUniform1i("u_AA", 0);

		program->SetVertexAttrib4f("a_color", 1.f, 1.f, 1.f, 1.f);

		glEnableVertexAttribArray(0);

		inner_->bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glDrawArrays(GL_TRIANGLE_FAN, 0,
						inner_->get_buffer_size()
										/ (2 * sizeof(GLfloat)));

		program->SetVertexAttrib4fv("a_color", Theme::instance->scroll().outline.data());
		program->SetUniform1i("u_AA", 1);

		outer_->bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0,
						outer_->get_buffer_size()
						/ (2 * sizeof(GLfloat)));
		outer_->reset();

		glDisableVertexAttribArray(0);
		program->reset();

		glBindVertexArray(0);
	}

}
