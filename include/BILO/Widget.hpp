/*
 * This file is part of BILO (Blender-like Interface Library in
 * OpenGL).
 *
 * BILO (Blender-like Interface Library in OpenGL) is free software:
 * you can redistribute it and/or modify it under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * BILO (Blender-like Interface Library in OpenGL) is distributed in
 * the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BILO.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Contributor(s): Freeman Zhang <zhanggyb@gmail.com>
 */

#ifndef _BILO_WIDGET_HPP_
#define _BILO_WIDGET_HPP_

#include <BILO/Drawable.hpp>
#include <BILO/Theme.hpp>
#include <BILO/Color.hpp>
#include <BILO/Theme.hpp>
#include <BILO/Rect.hpp>
#include <BILO/GLBuffer.hpp>

#include <Cpp/Events.hpp>

#define WIDGET_AA_JITTER 8

/* max as used by round_box__edges */
#define WIDGET_CURVE_RESOLU 9
#define WIDGET_SIZE_MAX (WIDGET_CURVE_RESOLU * 4)

namespace BILO {

	class Widget: public Drawable
	{
		DISALLOW_COPY_AND_ASSIGN(Widget);

	public:

		Widget ();

		Widget (Drawable* parent);

		virtual ~Widget ();

		const Padding& padding () const;

		void set_padding (const Padding& padding);

		void set_padding (int left, int right, int top, int bottom);

	protected:

		virtual bool update (int type, const void* property);

		virtual void render ();

		virtual void press_key (KeyEvent* event);

		virtual void press_context_menu (ContextMenuEvent* event);

		virtual void release_context_menu (ContextMenuEvent* event);

		virtual void press_mouse (MouseEvent* event);

		virtual void release_mouse (MouseEvent* event);

		virtual void move_mouse (MouseEvent* event);

		Padding m_padding;

		/**
		 * @brief get the GLBuffer
		 * @return
		 */
		GLBuffer& buffer () {return m_buffer;}

		Cpp::ConnectionScope m_events;

		GLBuffer m_buffer;

		static const float quarter_corner_vertexes[9][2];

	private:

		void update_shape (const Size* size);

		void draw_inner ();

		void draw_outline ();

		int m_vertex_num;

		int m_halfwayvert;

		/**
		 * vertexes for drawing shape
		 */
		//float m_vertexes[36][2];

		float m_outer_v[WIDGET_SIZE_MAX][2];
		float m_inner_v[WIDGET_SIZE_MAX][2];
	};

} /* namespace BILO */
#endif /* _BIL_WIDGET_H_ */
