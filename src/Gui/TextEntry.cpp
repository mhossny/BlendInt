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

#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <BlendInt/Gui/TextEntry.hpp>
#include <BlendInt/Gui/VertexTool.hpp>

#include <BlendInt/Stock/Shaders.hpp>
#include <BlendInt/Stock/Theme.hpp>

namespace BlendInt {

	Margin TextEntry::default_textentry_padding = Margin(2, 2, 2, 2);

	TextEntry::TextEntry ()
	: AbstractWidget(),
	  m_start(0),
	  m_length(0),
	  m_cursor_position(0)
	{
		InitializeTextEntry();
	}

	TextEntry::~TextEntry ()
	{
		glDeleteVertexArrays(3, m_vao);
	}

	void TextEntry::SetText (const String& text)
	{
		bool cal_width = true;

		m_text = text;

		Rect text_outline = m_font.GetTextOutline(m_text);

		m_length = m_text.length();

		if(size().height() < text_outline.height()) {
			m_length = 0;
			cal_width = false;
		}

		if(cal_width) {
			if(size().width() < text_outline.width()) {
				m_length = GetValidTextSize();
			}
		}

		m_font.set_pen(2,
				(size().height() - m_font.GetHeight()) / 2 + std::abs(m_font.GetDescender()));
	}

	void TextEntry::SetFont (const Font& font)
	{
		m_font = font;

		//Rect text_outline = m_font.GetTextOutline(m_text);

		//m_length = GetVisibleTextLengthInCursorMove(m_text, m_start);

		m_font.set_pen(default_textentry_padding.left(),
				(size().height() - m_font.GetHeight()) / 2 + std::abs(m_font.GetDescender()));
	}

	void TextEntry::Clear ()
	{
		m_text.clear();
		m_cursor_position = 0;
		m_start = 0;
		m_length = 0;
	}

	Size TextEntry::GetPreferredSize () const
	{
		Size preferred_size;

		int radius_plus = 0;

		if((round_type() & RoundTopLeft) || (round_type() & RoundBottomLeft)) {
			radius_plus += round_radius();
		}

		if((round_type() & RoundTopRight) || (round_type() & RoundBottomRight)) {
			radius_plus += round_radius();
		}

		int max_font_height = m_font.GetHeight();

		preferred_size.set_height(max_font_height + default_textentry_padding.vsum());	// top padding: 2, bottom padding: 2

		if (text().empty()) {
			preferred_size.set_width(
							max_font_height + default_textentry_padding.hsum()
											+ radius_plus + 120);
		} else {
			int width = m_font.GetTextWidth(text());
			preferred_size.set_width(width
									 + default_textentry_padding.hsum()
									 + radius_plus);	// left padding: 2, right padding: 2
		}

		return preferred_size;
	}

	bool TextEntry::IsExpandX () const
	{
		return true;
	}

	ResponseType TextEntry::KeyPressEvent (const KeyEvent& event)
	{
		if(!event.text().empty()) {
			m_text.insert(m_cursor_position, event.text());
			m_cursor_position += event.text().length();
			m_length += event.text().length();

			int text_width = m_font.GetTextWidth(m_text, m_length,
							m_start);
			int valid_width = size().width()
							- default_textentry_padding.hsum();

			if(text_width > valid_width) {
				m_start++;
				m_length--;

				text_width = m_font.GetTextWidth(m_text, m_length, m_start);
				while (text_width > valid_width) {
					m_start++;
					m_length--;
					text_width = m_font.GetTextWidth(m_text, m_length, m_start);
				}
			}

			Refresh();
			return Accept;

		} else {

			switch (event.key()) {

				case Key_Backspace: {
					DisposeBackspacePress();
					break;
				}

				case Key_Delete: {
					DisposeDeletePress();
					break;
				}

				case Key_Left: {
					DisposeLeftPress();
					break;
				}

				case Key_Right: {
					DisposeRightPress();
					break;
				}

				case Key_Up: {
					break;
				}

				case Key_Down: {
					break;
				}

				default:
					break;
			}

			return Accept;
		}
	}

	ResponseType TextEntry::MousePressEvent(const MouseEvent& event)
	{
		if(m_text.size()) {
			m_cursor_position = GetCursorPosition(event);
			Refresh();
		}

		return Accept;
	}

	void TextEntry::PerformSizeUpdate (const SizeUpdateRequest& request)
	{
		if (request.target() == this) {

			const Color& color = Theme::instance->text().inner;
			short shadetop = Theme::instance->text().shadetop;
			short shadedown = Theme::instance->text().shadedown;

			VertexTool tool;
			tool.Setup(*request.size(), DefaultBorderWidth(), round_type(),
			        round_radius(), color, Vertical, shadetop, shadedown);
			m_inner->Bind();
			tool.SetInnerBufferData(m_inner.get());
			m_outer->Bind();
			tool.SetOuterBufferData(m_outer.get());

			m_cursor_buffer->Bind();
			GLfloat* buf_p = (GLfloat*) m_cursor_buffer->Map(
			GL_READ_WRITE);
			*(buf_p + 5) = static_cast<float>(request.size()->height()
			        - default_textentry_padding.vsum());
			*(buf_p + 7) = static_cast<float>(request.size()->height()
			        - default_textentry_padding.vsum());
			m_cursor_buffer->Unmap();
			m_cursor_buffer->Reset();

			set_size(*request.size());
			Refresh();

		}

		ReportSizeUpdate(request);
	}

	void TextEntry::PerformRoundTypeUpdate (
	        const RoundTypeUpdateRequest& request)
	{
		if (request.target() == this) {

			const Color& color = Theme::instance->text().inner;
			short shadetop = Theme::instance->text().shadetop;
			short shadedown = Theme::instance->text().shadedown;

			VertexTool tool;
			tool.Setup(size(), DefaultBorderWidth(), *request.round_type(),
			        round_radius(), color, Vertical, shadetop, shadedown);
			m_inner->Bind();
			tool.SetInnerBufferData(m_inner.get());
			m_outer->Bind();
			tool.SetOuterBufferData(m_outer.get());

			set_round_type(*request.round_type());
			Refresh();
		}

		ReportRoundTypeUpdate(request);
	}

	void TextEntry::PerformRoundRadiusUpdate (
	        const RoundRadiusUpdateRequest& request)
	{
		if (request.target() == this) {
			const Color& color = Theme::instance->text().inner;
			short shadetop = Theme::instance->text().shadetop;
			short shadedown = Theme::instance->text().shadedown;

			VertexTool tool;
			tool.Setup(size(), DefaultBorderWidth(), round_type(),
			        *request.round_radius(), color, Vertical, shadetop,
			        shadedown);
			m_inner->Bind();
			tool.SetInnerBufferData(m_inner.get());
			m_outer->Bind();
			tool.SetOuterBufferData(m_outer.get());

			m_font.set_pen(
			        *request.round_radius() + default_textentry_padding.left(),
			        m_font.pen().y());

			set_round_radius(*request.round_radius());
			Refresh();
		}

		ReportRoundRadiusUpdate(request);
	}

	ResponseType TextEntry::Draw (const RedrawEvent& event)
	{
		using Stock::Shaders;

		RefPtr<GLSLProgram> program = Shaders::instance->triangle_program();
		program->Use();

		program->SetUniform3f("u_position", (float) position().x(), (float) position().y(), 0.f);
		program->SetUniform1i("u_gamma", 0);
		program->SetUniform1i("u_AA", 0);

		glBindVertexArray(m_vao[0]);
		glDrawArrays(GL_TRIANGLE_FAN, 0,
						GetOutlineVertices(round_type()) + 2);

		program->SetVertexAttrib4fv("a_color", Theme::instance->text().outline.data());
		program->SetUniform1i("u_AA", 1);

		glBindVertexArray(m_vao[1]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, GetOutlineVertices(round_type()) * 2 + 2);

		m_font.Print(position(), m_text, m_length, m_start);

		if(focused()) {			// draw a cursor
			unsigned int cursor_pos = m_font.GetTextWidth(m_text,
						        m_cursor_position - m_start, m_start);

			glm::vec3 pos(position().x() + cursor_pos + 1, position().y() + 1, 0.f);

			//program = ShaderManager::instance->default_line_program();	// Now switch to line program
			program->Use();

			program->SetUniform3fv("u_position", 1, glm::value_ptr(pos));
			program->SetUniform1i("u_gamma", 0);
			program->SetUniform1i("u_AA", 0);

			program->SetVertexAttrib4f("a_color",	0.f, 55 / 255.f, 1.f, 175 / 255.f);

			glBindVertexArray(m_vao[2]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		glBindVertexArray(0);
		program->Reset();

		return Accept;
	}

	ResponseType TextEntry::FocusEvent (bool focus)
	{
		Refresh();
		return Accept;
	}

	void TextEntry::InitializeTextEntry ()
	{
		int h = m_font.GetHeight();

		set_size(h + round_radius() * 2 + default_textentry_padding.hsum() + 120,
						h + default_textentry_padding.vsum());

		const Color& color = Theme::instance->text().inner;
		short shadetop = Theme::instance->text().shadetop;
		short shadedown = Theme::instance->text().shadedown;

		VertexTool tool;
		tool.Setup (size(),
						DefaultBorderWidth(),
						round_type(),
						round_radius(),
						color,
						Vertical,
						shadetop,
						shadedown);

		glGenVertexArrays(3, m_vao);

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

		std::vector<GLfloat> cursor_vertices(8);

		cursor_vertices[0] = 1.f;
		cursor_vertices[1] = static_cast<float>(default_textentry_padding.bottom());

		cursor_vertices[2] = 3.f;
		cursor_vertices[3] = static_cast<float>(default_textentry_padding.bottom());

		cursor_vertices[4] = 1.f;
		cursor_vertices[5] = static_cast<float>(size().height() - default_textentry_padding.vsum());

		cursor_vertices[6] = 3.f;
		cursor_vertices[7] = static_cast<float>(size().height() - default_textentry_padding.vsum());

		glBindVertexArray(m_vao[2]);
		m_cursor_buffer.reset(new GLArrayBuffer);

		m_cursor_buffer->Generate();
		m_cursor_buffer->Bind();
		m_cursor_buffer->SetData(8 * sizeof(GLfloat), &cursor_vertices[0]);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2,	GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		GLArrayBuffer::Reset();
		glBindVertexArray(0);

		m_font.set_pen(default_textentry_padding.left(),
				(size().height() - m_font.GetHeight()) / 2
		                + std::abs(m_font.GetDescender()));
	}

	size_t TextEntry::GetValidTextSize ()
	{
		int width = 0;
		int str_len = m_text.length();

		width = m_font.GetTextWidth(m_text, str_len, 0);

		if(width > size().width()) {
			while(str_len > 0) {
				width = m_font.GetTextWidth(m_text, str_len, 0);
				if(width < size().width()) break;
				str_len--;
			}
		}

		return str_len;
	}
	
	void TextEntry::DisposeBackspacePress ()
	{
		if (m_text.size() && m_cursor_position > 0) {

			size_t valid_width = size().width()
							- default_textentry_padding.hsum();

			m_text.erase(m_cursor_position - 1, 1);
			m_cursor_position--;

			size_t text_width = 0;
			size_t len = m_text.length();
			while(len > 0)
			{
				text_width = m_font.GetReversedTextWidth(m_text, len, 0);
				if(text_width < valid_width) {
					break;
				}
				len--;
			}

			m_length = len;
			m_start = m_text.length() - m_length;

			Refresh();
		}
	}
	
	void TextEntry::DisposeDeletePress ()
	{
		if (m_text.size() && (m_cursor_position < m_text.length())) {

			size_t valid_width = size().width()
							- default_textentry_padding.hsum();

			m_text.erase(m_cursor_position, 1);

			size_t text_width = 0;

			size_t len = 0;
			while(len < (m_text.length() - m_start))
			{
				text_width = m_font.GetTextWidth(m_text, len, m_start);
				if(text_width > valid_width) {
					len--;
					break;
				}
				len++;
			}

			m_length = len;

			Refresh();
		}
	}
	
	void TextEntry::DisposeLeftPress ()
	{
		if (m_text.size() && m_cursor_position > 0) {

			size_t valid_width = size().width()
								- default_textentry_padding.left()
								- default_textentry_padding.right();

			m_cursor_position--;

			if (m_cursor_position < m_start) {
				m_start = m_cursor_position;

				size_t text_width = m_font.GetTextWidth(m_text, m_length,
								m_start);

				if(text_width < valid_width && m_length < (m_text.length() - m_start)) {
					m_length++;
					text_width = m_font.GetTextWidth(m_text, m_length, m_start);
					while(text_width < valid_width && m_length < (m_text.length() - m_start)) {
						m_length++;
						text_width = m_font.GetTextWidth(m_text, m_length, m_start);
					}
				}

				if(text_width > valid_width && m_length > 0) {
					m_length--;
					text_width = m_font.GetTextWidth(m_text, m_length, m_start);
					while ((text_width > valid_width) && (m_length > 0)) {
						m_length--;
						text_width = m_font.GetTextWidth(m_text, m_length, m_start);
					}
				}

			}

			Refresh();
		}

	}
	
	void TextEntry::DisposeRightPress ()
	{
		if (m_text.size() && m_cursor_position < m_text.length()) {
			m_cursor_position++;

			if (m_cursor_position > (m_start + m_length))
				m_start++;

			//m_length = GetVisibleTextLength(m_text, m_start);

			//DBG_PRINT_MSG("length: %lu, start: %lu, cursor: %lu",
			//				m_length, m_start, m_cursor_position);

			Refresh();
		}
	}

	ResponseType TextEntry::CursorEnterEvent (bool entered)
	{
		return Ignore;
	}

	ResponseType TextEntry::ContextMenuPressEvent (
	        const ContextMenuEvent& event)
	{
		return Ignore;
	}

	ResponseType TextEntry::ContextMenuReleaseEvent (
	        const ContextMenuEvent& event)
	{
		return Ignore;
	}

	ResponseType TextEntry::MouseReleaseEvent (const MouseEvent& event)
	{
		return Ignore;
	}

	ResponseType TextEntry::MouseMoveEvent (const MouseEvent& event)
	{
		return Ignore;
	}

	void TextEntry::GetVisibleTextPlace (size_t* start, size_t* length)
	{
		int str_len = m_text.length();
		int width = m_font.GetTextWidth(m_text, str_len, 0);

		if(width < (size().width() - 4)) {
			*start = 0;
			*length = str_len;
		} else {
			while(str_len > 0) {
				str_len--;
				width = m_font.GetTextWidth(m_text, str_len, 0);
				if(width < (size().width() - 4)) break;
			}

			*start = m_text.length() - str_len;
			*length = str_len;
		}
	}
	
	int TextEntry::GetCursorPosition (const MouseEvent& event)
	{
		int text_width = m_font.GetTextWidth(m_text, m_length, m_start);
		int click_width = event.position().x() - position().x()
						- default_textentry_padding.left();

		if(click_width < 0 ||
		   click_width > (size().width() - default_textentry_padding.right())) {
			return m_cursor_position;
		}

		int cursor_offset = 1;

		text_width = m_font.GetTextWidth(m_text, cursor_offset, m_start);
		if(text_width > click_width) {
			cursor_offset--;
		} else {
			cursor_offset++;
			text_width = m_font.GetTextWidth(m_text, cursor_offset, m_start);
			while((text_width < click_width) &&
				  (cursor_offset <= static_cast<int>(m_length))) {
				cursor_offset++;
				text_width = m_font.GetTextWidth(m_text, cursor_offset, m_start);
			}
		}

		return m_start + cursor_offset;
	}

}
