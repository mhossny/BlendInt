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

#ifdef __UNIX__
#ifdef __APPLE__
#include <gl3.h>
#include <glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif  // __UNIX__

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <BlendInt/Service/ShaderManager.hpp>

#include <BlendInt/Gui/ToolButton.hpp>

namespace BlendInt {

	ToolButton::ToolButton ()
	: m_vao(0)
	{
		InitOnce();
	}

	ToolButton::~ToolButton ()
	{
		glDeleteVertexArrays(1, &m_vao);
	}

	ResponseType ToolButton::Draw (const RedrawEvent& event)
	{
		glm::vec3 pos((float) position().x(), (float) position().y(),
						(float) z());
		glm::mat4 mvp = glm::translate(event.projection_matrix() * event.view_matrix(), pos);

		RefPtr<GLSLProgram> program = ShaderManager::instance->default_triangle_program();

		glBindVertexArray(m_vao);

		program->Use();

		program->SetUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(mvp));
		program->SetUniform1i("AA", 0);

		if(down()) {
			program->SetVertexAttrib4f("Color", 0.2f, 0.2f, 0.2f, 1.f);
			program->SetUniform1i("Gamma", 0);
		} else {
			program->SetVertexAttrib4f("Color", 0.2f, 0.4f, 0.3f, 1.f);

			if(hover()) {
				program->SetUniform1i("Gamma", 20);
			} else {
				program->SetUniform1i("Gamma", 0);
			}
		}

		glEnableVertexAttribArray(0);

		m_inner->Bind();

		glVertexAttribPointer(0, // attribute
							  2,			// number of elements per vertex, here (x,y)
							  GL_FLOAT,			 // the type of each element
							  GL_FALSE,			 // take our values as-is
							  0,				 // no extra data between each position
							  0					 // offset of first element
							  );

		glDrawArrays(GL_TRIANGLE_FAN, 0,
							m_inner->GetBufferSize()
											/ (2 * sizeof(GLfloat)));

		m_inner->Reset();

		program->SetUniform1i("AA", 1);
		program->SetUniform1i("Gamma", 0);
		program->SetVertexAttrib4f("Color", 0.f, 0.f, 0.f, 1.f);

		m_outer->Bind();

		glVertexAttribPointer(0, // attribute
							  2,			// number of elements per vertex, here (x,y)
							  GL_FLOAT,			 // the type of each element
							  GL_FALSE,			 // take our values as-is
							  0,				 // no extra data between each position
							  0					 // offset of first element
							  );

		glDrawArrays(GL_TRIANGLE_STRIP, 0,
							m_outer->GetBufferSize()
											/ (2 * sizeof(GLfloat)));
		m_outer->Reset();

		glDisableVertexAttribArray(0);

		program->Reset();

		glBindVertexArray(0);

		return Accept;
	}

	void ToolButton::SetActionItem (const String& text)
	{
		RefPtr<ActionItem> action(new ActionItem(text));

		m_action = action;
	}

	void ToolButton::SetActionItem (const String& text, const String& shortcut)
	{
		RefPtr<ActionItem> action(new ActionItem(text, shortcut));

		m_action = action;
	}

	void ToolButton::SetActionItem (const RefPtr<Icon>& icon, const String& text)
	{
		RefPtr<ActionItem> action(new ActionItem(icon, text));

		m_action = action;
	}

	void ToolButton::SetActionItem (const RefPtr<Icon>& icon, const String& text,
	        const String& shortcut)
	{
		RefPtr<ActionItem> action(new ActionItem(icon, text, shortcut));

		m_action = action;
	}

	void ToolButton::SetActionItem (const RefPtr<ActionItem>& item)
	{
		m_action = item;
	}

	void ToolButton::InitOnce ()
	{
		set_preferred_size(24, 24);
		set_size(24, 24);

		m_inner.reset(new GLArrayBuffer);
		m_outer.reset(new GLArrayBuffer);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		m_inner->Generate();
		m_outer->Generate();

		GenerateFormBuffer(size(), RoundAll, 5.0, m_inner.get(), m_outer.get(), 0);

		glBindVertexArray(0);
	}

}
