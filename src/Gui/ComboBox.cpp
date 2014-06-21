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

#include <math.h>

#ifdef __UNIX__
#ifdef __APPLE__
#include <gl3.h>
#include <gl3ext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif  // __UNIX__

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <BlendInt/Gui/ComboBox.hpp>
#include <BlendInt/Gui/VertexTool.hpp>
#include <BlendInt/Stock/Shaders.hpp>
#include <BlendInt/Stock/Theme.hpp>
#include <BlendInt/Stock/Icons.hpp>

#include <BlendInt/Gui/Context.hpp>

namespace BlendInt {

	Margin ComboBox::default_combobox_padding = Margin(2, 2, 2, 2);

	ComboBox::ComboBox ()
	: Widget(),
	  m_status_down(false)
	{
		set_round_corner_type(RoundAll);

		int h = m_font.GetHeight();

		set_size(
		        h + round_corner_radius() * 2 + default_combobox_padding.hsum() + 100,
		        h + default_combobox_padding.vsum());

		InitializeComboBox();
	}

	ComboBox::~ComboBox ()
	{
		glDeleteVertexArrays(2, m_vao);
	}

	Size ComboBox::GetPreferredSize () const
	{
		Size preferred_size;

		int radius_plus = 0;

		if((round_corner_type() & RoundTopLeft) || (round_corner_type() & RoundBottomLeft)) {
			radius_plus += round_corner_radius();
		}

		if((round_corner_type() & RoundTopRight) || (round_corner_type() & RoundBottomRight)) {
			radius_plus += round_corner_radius();
		}

		int max_font_height = m_font.GetHeight();

		preferred_size.set_height(max_font_height + default_combobox_padding.vsum());	// top padding: 2, bottom padding: 2

		if (m_text.empty()) {
			preferred_size.set_width(
							max_font_height + default_combobox_padding.hsum()
											+ radius_plus + 100);
		} else {
			size_t width = m_font.GetTextWidth(m_text);
			preferred_size.set_width(width
							+ default_combobox_padding.hsum()
							+ radius_plus);	// left padding: 2, right padding: 2
		}

		return preferred_size;
	}

	bool ComboBox::IsExpandX() const
	{
		return true;
	}

	void ComboBox::UpdateGeometry (const WidgetUpdateRequest& request)
	{
		switch (request.type()) {

			case WidgetSize: {
				const Size* size_p = static_cast<const Size*>(request.data());
				VertexTool tool;
				tool.Setup(*size_p,
								DefaultBorderWidth(),
								round_corner_type(),
								round_corner_radius(),
								Theme::instance->menu().inner,
								Vertical,
								Theme::instance->menu().shadetop,
								Theme::instance->menu().shadedown);
				m_inner->Bind();
				tool.SetInnerBufferData(m_inner.get());
				m_outer->Bind();
				tool.SetOuterBufferData(m_outer.get());
				Refresh();
				break;
			}

			case WidgetRoundCornerType: {
				const int* type_p = static_cast<const int*>(request.data());
				VertexTool tool;
				tool.Setup(size(),
								DefaultBorderWidth(),
								*type_p,
								round_corner_radius(),
								Theme::instance->menu().inner,
								Vertical,
								Theme::instance->menu().shadetop,
								Theme::instance->menu().shadedown);
				m_inner->Bind();
				tool.SetInnerBufferData(m_inner.get());
				m_outer->Bind();
				tool.SetOuterBufferData(m_outer.get());
				Refresh();
				break;
			}

			case WidgetRoundCornerRadius: {
				const float* radius_p =
								static_cast<const float*>(request.data());
				VertexTool tool;
				tool.Setup(size(),
								DefaultBorderWidth(),
								round_corner_type(),
								*radius_p,
								Theme::instance->menu().inner,
								Vertical,
								Theme::instance->menu().shadetop,
								Theme::instance->menu().shadedown);
				m_inner->Bind();
				tool.SetInnerBufferData(m_inner.get());
				m_outer->Bind();
				tool.SetOuterBufferData(m_outer.get());
				Refresh();
				break;
			}

			default:
				Widget::UpdateGeometry(request);
		}

	}

	ResponseType ComboBox::Draw(const RedrawEvent& event)
	{
		using Stock::Shaders;

		glm::vec3 pos((float) position().x(), (float) position().y(),
						(float) z());
		glm::mat4 mvp = glm::translate(
						event.projection_matrix() * event.view_matrix(), pos);

		RefPtr<GLSLProgram> program = Shaders::instance->default_triangle_program();
		program->Use();

		program->SetUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(mvp));
		program->SetUniform1i("AA", 0);

		if(m_status_down) {
			program->SetUniform1i("Gamma", 20);
		} else {
			if(hover()) {
				program->SetUniform1i("Gamma", 15);
			} else {
				program->SetUniform1i("Gamma", 0);
			}
		}

		glBindVertexArray(m_vao[0]);
		glDrawArrays(GL_TRIANGLE_FAN, 0,
						GetOutlineVertices(round_corner_type()) + 2);

		program->SetVertexAttrib4fv("Color", Theme::instance->menu().outline.data());
		program->SetUniform1i("AA", 1);
		program->SetUniform1i("Gamma", 0);

		glBindVertexArray(m_vao[1]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, GetOutlineVertices(round_corner_type()) * 2 + 2);

		glBindVertexArray(0);
		program->Reset();

		RefPtr<VertexIcon> icon = Stock::Icons::instance->icon_num();

		glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(1.15, 1.15, 1.15));
		glm::mat4 rotate = glm::rotate(glm::mat4(1.0), (glm::mediump_float)(M_PI * 1.5), glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(icon->size().width()/2.f, icon->size().height()/2.f, 0.0));

		icon->Draw(mvp * translate * rotate * scale);

		return Accept;
	}
	
	ResponseType ComboBox::MousePressEvent (const MouseEvent& event)
	{
		m_status_down = true;

		Context* context = GetContext();
		if(context) {
			if(m_menu->container()) {
				context->Remove(m_menu.get());
				SetRoundCornerType(RoundAll);
			} else {
				int max_layer = context->GetMaxLayer();
				m_menu->SetLayer(max_layer + 1);
				m_menu->SetPosition(position().x(), position().y() + size().height());
				context->Add(m_menu.get());
				SetRoundCornerType(RoundBottomLeft | RoundBottomRight);
				//context->SetFocusedWidget(m_menu.get());	// FIXME: no use, context will reset to this combobox.
			}

			Refresh();
		}

		return Accept;
	}
	
	ResponseType ComboBox::MouseReleaseEvent (const MouseEvent& event)
	{
		m_status_down = false;

		Refresh();
		return Accept;
	}
	
	ResponseType ComboBox::CursorEnterEvent (bool entered)
	{
		Refresh();
		return Accept;
	}

	void ComboBox::InitializeComboBox()
	{
		glGenVertexArrays(2, m_vao);

		VertexTool tool;
		tool.Setup(size(),
						DefaultBorderWidth(),
						round_corner_type(),
						round_corner_radius(),
						Theme::instance->menu().inner,
						Vertical,
						Theme::instance->menu().shadetop,
						Theme::instance->menu().shadedown);

		glBindVertexArray(m_vao[0]);
		m_inner.reset(new GLArrayBuffer);
		m_inner->Generate();
		m_inner->Bind();
		tool.SetInnerBufferData(m_inner.get());

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 2,	GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, BUFFER_OFFSET(2 * sizeof(GLfloat)));

		glBindVertexArray(m_vao[1]);
		m_outer.reset(new GLArrayBuffer);
		m_outer->Generate();
		m_outer->Bind();
		tool.SetOuterBufferData(m_outer.get());

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2,	GL_FLOAT, GL_FALSE, 0, 0);

		GLArrayBuffer::Reset();
		glBindVertexArray(0);

		// Now create menu

		m_menu.reset(new Menu);

		m_menu->SetRoundCornerType(RoundTopLeft | RoundTopRight);
		//m_menu->SetPosition(200, 200);
		//menu->Resize (200, 200);

		m_menu->AddAction(Stock::Icons::instance->icon_check(), "MenuItem1", "Ctrl + 1");
		m_menu->AddAction("MenuItem2", "Ctrl + 1");
		m_menu->AddAction("MenuItem3", "Ctrl + 1");
		m_menu->AddAction("MenuItem4", "Ctrl + 1");
		m_menu->AddAction("MenuItem5");

		events()->connect(m_menu->triggered(), this, &ComboBox::OnMenuActionTriggered);
	}

	void ComboBox::OnMenuActionTriggered (Action* item)
	{
		Context* context = GetContext();

		context->Remove(m_menu.get());
		SetRoundCornerType(RoundAll);

		Refresh();
	}

}
