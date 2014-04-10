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

#ifndef _BLENDINT_CHECKBUTTON_HPP_
#define _BLENDINT_CHECKBUTTON_HPP_

#include <BlendInt/OpenGL/GLArrayBuffer.hpp>
#include <BlendInt/Gui/AbstractButton.hpp>
#include <BlendInt/Gui/RoundShapeBase.hpp>

namespace BlendInt {

	class String;

	/**
	 * @brief Toggle Button
	 *
	 * @ingroup widgets
	 */
	class ToggleButton: public AbstractButton, public RoundShapeBase
	{
		DISALLOW_COPY_AND_ASSIGN(ToggleButton);

	public:

		ToggleButton ();

		ToggleButton (const String& text);

		virtual ~ToggleButton ();

		/**
		 * @brief set round type
		 * @param type
		 *
		 * Hide the same function in RoundBoxBase to call Update
		 */
		void SetRoundType (int type);

		/**
		 * @brief set round radius
		 * @param radius
		 * Hide the same function in RoundBoxBase to call Update
		 */
		void SetRadius (float radius);

	protected:

		virtual bool Update (const UpdateRequest& request);

		virtual ResponseType Draw (const RedrawEvent& event);

	private:

		void InitializeOnce ();

		void InitializeOnce (const String& text);

		GLuint m_vao;

		RefPtr<GLArrayBuffer> m_inner_buffer;
		RefPtr<GLArrayBuffer> m_outer_buffer;
		RefPtr<GLArrayBuffer> m_emboss_buffer;

	};

}

#endif /* _BIL_CHECKBUTTON_HPP_ */
