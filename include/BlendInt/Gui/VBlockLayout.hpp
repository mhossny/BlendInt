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

#ifndef _BLENDINT_GUI_VBLOCKLAYOUT_HPP_
#define _BLENDINT_GUI_VBLOCKLAYOUT_HPP_

#include <BlendInt/Gui/Layout.hpp>

namespace BlendInt {

	/**
	 * @brief A container looks like an entirety
	 */
	class VBlockLayout: public Layout
	{
		DISALLOW_COPY_AND_ASSIGN(VBlockLayout);

	public:

		VBlockLayout ();

		virtual ~VBlockLayout ();

		void Prepend (Widget* widget);

		void Append (Widget* widget);

		virtual bool IsExpandX () const;

		virtual bool IsExpandY () const;

		virtual Size GetPreferredSize () const;

	protected:

		virtual void PerformMarginUpdate (const Margin& request);

		virtual bool SizeUpdateTest (const SizeUpdateRequest& request);

		virtual bool PositionUpdateTest (const PositionUpdateRequest& request);

		virtual void PerformSizeUpdate (const SizeUpdateRequest& request);

		void FillInVBlock (const Size& out_size, const Margin& margin);

		void FillInVBlock (int x, int y, int w, int h);

	private:

	};

}

#endif /* _BLENDINT_GUI_VBLOCKLAYOUT_HPP_ */
