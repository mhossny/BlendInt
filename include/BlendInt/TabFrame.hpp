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

#ifndef _BLENDINT_TABFRAME_HPP_
#define _BLENDINT_TABFRAME_HPP_

#include <BlendInt/Frame.hpp>
#include <BlendInt/StackedWidget.hpp>
#include <BlendInt/HorizontalLayout.hpp>
#include <BlendInt/VerticalLayout.hpp>

namespace BlendInt {

	class String;

	class TabFrame: public Frame
	{
		DISALLOW_COPY_AND_ASSIGN(TabFrame);

	public:

		TabFrame ();

		TabFrame (AbstractWidget* parent);

		virtual ~TabFrame ();

		void Add (const String& title, Widget* widget);

	protected:

		virtual void Update (int type, const void* data);

		virtual void Render ();

		void KeyPressEvent (KeyEvent* event);

		void ContextMenuPressEvent (ContextMenuEvent* event);

		void ContextMenuReleaseEvent (ContextMenuEvent* event);

		void MousePressEvent (MouseEvent* event);

		void MouseReleaseEvent (MouseEvent* event);

		void MouseMoveEvent (MouseEvent* event);

	private:

		void Switch ();

		void Init ();

		StackedWidget* m_stack_widget;

		HorizontalLayout* m_button_layout;

		VerticalLayout* m_body_layout;
	};

}

#endif /* _BLENDINT_TABFRAME_HPP_ */
