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

#pragma once

#include <BlendInt/Core/Types.hpp>
#include <BlendInt/Gui/Widget.hpp>
#include <BlendInt/Gui/AbstractNode.hpp>
#include <BlendInt/Gui/CubicBezierCurve.hpp>

namespace BlendInt {

	class NodeView: public Widget
	{
		DISALLOW_COPY_AND_ASSIGN(NodeView);

	public:

		NodeView ();

		NodeView (int width, int height);

		virtual ~NodeView ();

		bool AddNode (AbstractNode* node);

		virtual bool IsExpandX () const;

		virtual bool IsExpandY () const;

		virtual Size GetPreferredSize () const;

	protected:

		virtual bool SizeUpdateTest (const SizeUpdateRequest& request);

		virtual bool PositionUpdateTest (const PositionUpdateRequest& request);

		virtual void PerformSizeUpdate (const SizeUpdateRequest& request);

		virtual void PerformRoundTypeUpdate (int round_type);

		virtual void PerformRoundRadiusUpdate (float radius);

		virtual ResponseType PerformMousePress (const Context* context);

		virtual ResponseType Draw (const Context* context);

	private:

		void InitializeNodeView ();

		//CubicBezierCurve* curve_;

		GLuint vao_;

		GLBuffer<> vbo_;

	};

}
