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
#include <gl3ext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif  // __UNIX__

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <BlendInt/Gui/ScrollBar.hpp>
#include <BlendInt/Service/Theme.hpp>
#include <BlendInt/Service/ShaderManager.hpp>

namespace BlendInt {

	ScrollBar::ScrollBar (Orientation orientation)
	: AbstractSlider<int>(orientation),
	  m_vao(0),
	  m_last_value(0),
	  m_pressed(false)
	{
		if (orientation == Horizontal) {
			m_slide.Resize(32, 14);
			set_size(200, 14);
		} else {
			m_slide.Resize(14, 32);
			set_size(14, 200);
		}

		InitOnce();
	}

	ScrollBar::~ScrollBar ()
	{
		glDeleteVertexArrays(1, &m_vao);
	}

	void ScrollBar::SetPercentage (int percentage)
	{
		if (percentage < 0 || percentage > 100) return;

		if(orientation() == Horizontal) {

			unsigned int w = percentage * size().width() / 100;

			if(w < m_slide.size().width())
				w = m_slide.size().width();

			m_slide.Resize(w, m_slide.size().height());

			Refresh();

		} else {

			unsigned int h = percentage * size().height() / 100;

			if(h < m_slide.size().height())
				h = m_slide.size().height();

			m_slide.Resize(m_slide.size().width(), h);

			Refresh();

		}
	}

	int ScrollBar::GetPercentage () const
	{
		int percentage = 0;

		if(orientation() == Horizontal) {
			percentage = m_slide.size().width() * 100 / size().width();
		} else {
			percentage = m_slide.size().height() * 100 / size().height();
		}

		return percentage;
	}

	bool ScrollBar::IsExpandX() const
	{
		if (orientation() == Horizontal) {
			return true;
		} else {
			return false;
		}

	}

	bool ScrollBar::IsExpandY() const
	{
		if(orientation() == Vertical) {
			return true;
		} else {
			return false;
		}
	}

	bool ScrollBar::UpdateGeometryTest (const WidgetUpdateRequest& request)
	{
		switch (request.type()) {

			case SliderPropertyMinimum: {

				const int* min_p = static_cast<const int*>(request.data());

				if (*min_p >= maximum())
					return false;

				return true;
			}

			case SliderPropertyMaximum: {

				const int* max_p = static_cast<const int*>(request.data());

				if (*max_p <= minimum())
					return false;

				return true;
			}

			default:
				return AbstractSlider<int>::UpdateGeometryTest(request);
		}
	}

	void ScrollBar::UpdateGeometry (const WidgetUpdateRequest& request)
	{
		switch (request.type()) {
			case WidgetPosition: {
				// don't care position change
				break;
			}

			case WidgetSize: {
				const Size* size_p = static_cast<const Size*>(request.data());

				int radius = std::min(size_p->width(), size_p->height()) / 2;

				Orientation slot_orient;
				if (orientation() == Vertical) {
					slot_orient = Horizontal;
					m_slide.Resize(radius * 2, m_slide.size().height());
					m_slide.SetRadius(radius);
				} else {
					slot_orient = Vertical;
					m_slide.Resize(m_slide.size().width(), radius * 2);
					m_slide.SetRadius(radius);
				}

				const Color& color = Theme::instance->scroll().inner;
				short shadetop = Theme::instance->scroll().shadetop;
				short shadedown = Theme::instance->scroll().shadedown;
				if (orientation() == Vertical) {
					shadetop = Theme::instance->scroll().shadedown;
					shadedown = Theme::instance->scroll().shadetop;
				}

				GenerateShadedFormBuffers(*size_p, RoundAll, radius, color,
								shadetop, shadedown, slot_orient,
								m_slot_inner_buffer.get(),
								m_slot_outline_buffer.get());

				break;
			}

			default:
				break;
		}

	}

	void ScrollBar::UpdateSlider(const WidgetUpdateRequest& request)
	{
		switch(request.type()) {

			case SliderPropertyValue: {

				break;
			}

			case SliderPropertyMinimum: {

				const int* min_p = static_cast<const int*>(request.data());

				if (value() < *min_p) {
					set_value(*min_p);
				}

				break;
			}

			case SliderPropertyMaximum: {

				const int* max_p = static_cast<const int*>(request.data());

				if (value() > *max_p) {
					set_value(*max_p);
				}

				break;
			}

			case SliderPropertyOrientation: {
				//const Orientation* orient_p =
				//			static_cast<const Orientation*>(request.data());

				// TODO:

				//Refresh();

				break;
			}

			default:
				break;
		}
	}

	ResponseType ScrollBar::Draw (const RedrawEvent& event)
	{
		glm::vec3 pos((float) position().x(), (float) position().y(),
						(float) z());
		glm::mat4 mvp = glm::translate(event.projection_matrix() * event.view_matrix(), pos);

		glm::mat4 local_mvp;

		glBindVertexArray(m_vao);

		RefPtr<GLSLProgram> program =
						ShaderManager::instance->default_triangle_program();
		program->Use();

		program->SetUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(mvp));

		program->SetUniform1i("AA", 0);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		DrawShadedTriangleFan(0, 1, m_slot_inner_buffer.get());

		glDisableVertexAttribArray(1);

		Color color = Theme::instance->scroll().outline;

		program->SetVertexAttrib4fv("Color", color.data());
		program->SetUniform1i("AA", 1);

		DrawTriangleStrip(0, m_slot_outline_buffer.get());

		glDisableVertexAttribArray(0);

		program->Reset();

		glBindVertexArray(0);

		if (orientation() == Horizontal) {
			local_mvp = glm::translate(mvp,
							glm::vec3(GetSlidePosition(), 0.f, 0.f));
		} else {
			local_mvp = glm::translate(mvp,
							glm::vec3(0.f, GetSlidePosition(), 0.f));
		}

		m_slide.Draw(local_mvp);

		return Accept;
	}

	ResponseType ScrollBar::MouseMoveEvent (const MouseEvent& event)
	{
		if (m_pressed) {

			int new_value = value();

			// DO not fire if cursor is out of range, otherwise too many events
			if (GetNewValue(event.position(), &new_value)) {
				set_value(new_value);
				fire_slider_moved_event(value());
				Refresh();
			}

			return Accept;

		} else {
			if (CursorOnSlideIcon(event.position())) {
				m_slide.set_highlight(true);

				Refresh();

				return Accept;
			} else {
				m_slide.set_highlight(false);
				Refresh();

				return Accept;
			}
		}
	}

	ResponseType ScrollBar::MousePressEvent (const MouseEvent& event)
	{
		if (CursorOnSlideIcon(event.position())) {

			m_cursor_origin = event.position();
			m_last_value = value();
			m_pressed = true;
			fire_slider_pressed();

			return Accept;
		} else {
			return Ignore;
		}
	}

	ResponseType ScrollBar::MouseReleaseEvent (const MouseEvent& event)
	{
		if (m_pressed) {
			m_pressed = false;

			if (CursorOnSlideIcon(event.position())) {
				fire_slider_released();
			}

			Refresh();
		}

		return Accept;
	}

	void ScrollBar::InitOnce ()
	{
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		m_slot_inner_buffer.reset(new GLArrayBuffer);
		m_slot_outline_buffer.reset(new GLArrayBuffer);

		Size slot_size = m_slide.size();
		float slot_radius;
		Orientation slot_orient;
		if(orientation() == Horizontal) {
			slot_size.set_width(size().width());
			slot_radius = m_slide.size().height() / 2.f;
			slot_orient = Vertical;
		} else {
			slot_size.set_height(size().height());
			slot_radius = m_slide.size().width() / 2.f;
			slot_orient = Horizontal;
		}

		const Color& color = Theme::instance->scroll().inner;
		short shadetop = Theme::instance->scroll().shadetop;
		short shadedown = Theme::instance->scroll().shadedown;
		if(orientation() == Vertical) {
			shadetop = Theme::instance->scroll().shadedown;
			shadedown = Theme::instance->scroll().shadetop;
		}

		GenerateShadedFormBuffers(
						slot_size,
						RoundAll,
						slot_radius,
						color,
						shadetop,
						shadedown,
						slot_orient,
						m_slot_inner_buffer.get(),
						m_slot_outline_buffer.get()
						);

		glBindVertexArray(0);
	}

	int ScrollBar::GetSpace ()
	{
		int space = 0;

		if (orientation() == Horizontal) {
			space = size().width() - m_slide.size().width();
		} else {
			space = size().height() - m_slide.size().height();
		}

		return space;
	}

	int ScrollBar::GetSlidePosition ()
	{
		int pos = 0;

		if(orientation() == Horizontal) {
			pos = (value () - minimum()) * (size().width() - m_slide.size().width()) / (maximum() - minimum());
		} else {
			pos = (value () - minimum()) * (size().height() - m_slide.size().height()) / (maximum() - minimum());
			pos = size().height() - m_slide.size().height() - pos;
		}

		return pos;
	}

	bool ScrollBar::CursorOnSlideIcon (const Point& cursor)
	{
		int slide_pos = GetSlidePosition();

		int xmin, ymin, xmax, ymax;

		if(orientation() == Horizontal) {
			xmin = position().x() + slide_pos;
			ymin = position().y() + (size().height() - m_slide.size().height()) / 2;
			xmax = xmin + m_slide.size().width();
			ymax = ymin + m_slide.size().height();
		} else {
			xmin = position().x() + (size().width() - m_slide.size().width()) / 2;
			ymin = position().y() + slide_pos;
			xmax = xmin + m_slide.size().width();
			ymax = ymin + m_slide.size().height();
		}

		if(cursor.x() < xmin ||
			cursor.y() < ymin ||
			cursor.x() > xmax ||
			cursor.y() > ymax)
		{
			return false;
		}

		glm::vec2 center;
		glm::vec2 cursor_pos(cursor.x(), cursor.y());
		float distance = 0.f;

		if(orientation() == Horizontal) {

			if(cursor.x() < (xmin + m_slide.radius())) {

				center.x = xmin + m_slide.radius();
				center.y = (ymax - ymin) / 2 + ymin;

				distance = glm::distance(center, cursor_pos);

				return distance <= m_slide.radius() ? true : false;

			} else if (cursor.x() > (xmax - m_slide.radius())) {

				center.x = xmax - m_slide.radius();
				center.y = (ymax - ymin) / 2 + ymin;

				distance = glm::distance(center, cursor_pos);

				return distance <= m_slide.radius() ? true : false;

			} else {
				return true;
			}

		} else {

			if(cursor.y() < (ymin + m_slide.radius())) {

				center.x = (xmax - xmin) / 2 + xmin;
				center.y = ymin + m_slide.radius();

				distance = glm::distance(center, cursor_pos);

				return distance <= m_slide.radius() ? true : false;

			} else if (cursor.y() > (ymax - m_slide.radius())) {

				center.x = (xmax - xmin) / 2 + xmin;
				center.y = ymax - m_slide.radius();

				distance = glm::distance(center, cursor_pos);

				return distance <= m_slide.radius() ? true : false;

			} else {
				return true;
			}

		}

		return false;
	}

	bool ScrollBar::GetNewValue (const Point& cursor, int* vout)
	{
		bool ret = false;

		int offset = 0;
		int move_space = GetSpace();

		if(move_space == 0)
			return false;

		if (orientation() == Horizontal) {
			offset = cursor.x() - m_cursor_origin.x();
		} else {
			offset = m_cursor_origin.y() - cursor.y();
		}

		int val = m_last_value + (offset * (maximum() - minimum())) / move_space;

		if(val > maximum()) {
			*vout = maximum();
		} else if(val < minimum()) {
			*vout = minimum();
		} else {
			*vout = val;
			ret = true;
		}

		return ret;
	}

}
