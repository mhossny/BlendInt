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

#ifndef _BLENDINT_GUI_SPLITTER_HPP_
#define _BLENDINT_GUI_SPLITTER_HPP_

#include <BlendInt/Gui/AbstractContainer.hpp>

namespace BlendInt
{

	class Splitter;

	class SplitterHandle: public AbstractWidget
	{
	DISALLOW_COPY_AND_ASSIGN(SplitterHandle);

	public:

		friend class Splitter;

		SplitterHandle (Orientation orientation);

		virtual ~SplitterHandle ();

		virtual Size GetPreferredSize () const;

		virtual bool IsExpandX () const;

		virtual bool IsExpandY () const;

	protected:

		virtual void PerformSizeUpdate (const SizeUpdateRequest& request);

		virtual ResponseType Draw (const Profile& profile);

		virtual ResponseType FocusEvent (bool focus);

		virtual ResponseType CursorEnterEvent (bool entered);

		virtual ResponseType KeyPressEvent (const KeyEvent& event);

		virtual ResponseType ContextMenuPressEvent (
		        const ContextMenuEvent& event);

		virtual ResponseType ContextMenuReleaseEvent (
		        const ContextMenuEvent& event);

		virtual ResponseType MousePressEvent (const MouseEvent& event);

		virtual ResponseType MouseReleaseEvent (const MouseEvent& event);

		virtual ResponseType MouseMoveEvent (const MouseEvent& event);

	private:

		Orientation orientation_;

		GLuint vao_;

		bool highlight_;
		bool pressed_;

		RefPtr<GLArrayBuffer> buffer_;

		Point last_;
		Point cursor_;

		int prev_size_;	// width or height of the previous widget
		int next_size_;	// width or height of the next widget
		int nearby_pos_;	// nearby widget position

		AbstractWidget* prev_widget_;
		AbstractWidget* next_widget_;

	};

	/**
	 * @brief Splitter container
	 *
	 * A Splitter lets the user control the size of the sub widgets by dragging
	 * the splitter handler between them.
	 */
	class Splitter: public AbstractContainer
	{
	DISALLOW_COPY_AND_ASSIGN(Splitter);

	public:

		friend class SplitterHandle;

		Splitter (Orientation orientation = Horizontal);

		virtual ~Splitter ();

		void PushFront (AbstractWidget* widget);

		void PushBack (AbstractWidget* widget);

		void Remove (AbstractWidget* widget);

		int GetWidgetIndex (AbstractWidget* widget) const;

		int GetHandleIndex (SplitterHandle* handle) const;

		AbstractWidget* GetWidget (int index) const;

		SplitterHandle* GetHandle (int index) const;

		int GetWidgetsHold () const;

		void MoveHandle (int index, int x, int y);

		virtual Size GetPreferredSize () const;

		virtual bool IsExpandX () const;

		virtual bool IsExpandY () const;

	protected:

		virtual void PerformMarginUpdate (const Margin& request);

		virtual void PerformPositionUpdate (const PositionUpdateRequest& request);

		virtual void PerformSizeUpdate (const SizeUpdateRequest& request);

		virtual ResponseType Draw (const Profile& profile);

		virtual ResponseType CursorEnterEvent (bool entered);

		virtual ResponseType KeyPressEvent (const KeyEvent& event);

		virtual ResponseType ContextMenuPressEvent (
		        const ContextMenuEvent& event);

		virtual ResponseType ContextMenuReleaseEvent (
		        const ContextMenuEvent& event);

		virtual ResponseType MousePressEvent (const MouseEvent& event);

		virtual ResponseType MouseReleaseEvent (const MouseEvent& event);

		virtual ResponseType MouseMoveEvent (const MouseEvent& event);

		void FillSubWidgetsInSplitter (const Point& out_pos,
		        const Size& out_size, const Margin& margin,
		        Orientation orientation);

		void FillSubWidgetsInSplitter (const Point& pos, const Size& size,
		        Orientation orientation);

		/**
		 * @brief Fill the sub widgets in this container based on the current size
		 * @param x
		 * @param y
		 * @param width
		 * @param height
		 * @param orientation
		 */
		void FillSubWidgetsInSplitter (int x, int y, int width, int height,
		        Orientation orientation);

	private:

		void DistributeHorizontally (int x, int width);

		void DistributeHorizontallyInProportion (int x, int width,
		        std::deque<int>* widget_deque, int widget_width_sum,
		        std::deque<int>* prefer_deque, int prefer_width_sum);

		void DistributeExpandableWidgetsHorizontally (int x, int width,
		        int unexpandable_width_sum, std::deque<int>* widget_deque,
		        int widget_width_sum, std::deque<int>* prefer_deque,
		        int prefer_width_sum);

		void DistributeUnexpandableWidgetsHorizontally (int x, int width,
		        std::deque<int>* widget_deque, int widget_width_sum,
		        std::deque<int>* prefer_deque, int prefer_width_sum);

		void DistributeVertically (int y, int height);

		void DistributeVerticallyInProportion (int y, int height,
		        std::deque<int>* widget_deque, int widget_height_sum,
		        std::deque<int>* prefer_deque, int prefer_height_sum);

		void DistributeExpandableWidgetsVertically (int y, int height,
		        int unexpandable_width_sum, std::deque<int>* widget_deque,
		        int widget_height_sum, std::deque<int>* prefer_deque,
		        int prefer_height_sum);

		void DistributeUnexpandableWidgetsVertically (int y, int height,
		        std::deque<int>* widget_deque, int widget_height_sum,
		        std::deque<int>* prefer_deque, int prefer_height_sum);

		void AlignHorizontally (int y, int height);

		void AlignVertically (int x, int width);

		void AlignSubWidgets (Orientation orientation, const Size& out_size,
		        const Margin& margin);

		int GetAverageRoom (Orientation orientation, const Size& out_size,
		        const Margin& margin);

		int GetWidgetsRoom (Orientation orientation, const Size& out_size,
		        const Margin& margin);

		Orientation orientation_;
	};
}

#endif /* _BLENDINT_GUI_SPLITTER_HPP_ */
