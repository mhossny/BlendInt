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

#include <boost/smart_ptr.hpp>
#include <BlendInt/Gui/HLayout.hpp>

namespace BlendInt {

	HBox::HBox (int align, int space)
	: AbstractDequeContainer(), m_alignment(align), m_space(space)
	{
		set_size (200, 200);
	}

	HBox::~HBox ()
	{
	}

	bool HBox::Add (AbstractWidget* widget)
	{
		bool ret = false;

		if(AddSubWidget(widget)) {

			//FillSubWidgetsAveragely(position(), size(), margin(), Horizontal, m_alignment, m_space);
			FillSubWidgetsInHBox(position(), size(), margin(), m_space);

			ret = true;
		}

		return ret;
	}

	bool HBox::Remove (AbstractWidget* widget)
	{
		bool ret = false;

		if(RemoveSubWidget(widget)) {

			FillSubWidgetsAveragely(position(), size(), margin(), Horizontal, m_alignment, m_space);

			ret = true;

		}

		return ret;
	}

	void HBox::SetAlignment (int align)
	{
	}

	void HBox::SetSpace (int space)
	{
	}

	Size BlendInt::HBox::GetPreferredSize () const
	{
		Size preferred_size;

		if(sub_widget_size() == 0) {

			preferred_size.set_width(200);
			preferred_size.set_height(200);

		} else {

			AbstractWidget* widget = 0;
			Size tmp_size;

			preferred_size.set_width(-m_space);
			for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
			{
				widget = *it;

				if(widget->visiable()) {
					tmp_size = widget->GetPreferredSize();

					preferred_size.add_width(tmp_size.width() + m_space);
					preferred_size.set_height(std::max(preferred_size.height(), tmp_size.height()));
				}
			}

			preferred_size.add_width(margin().left() + margin().right());
			preferred_size.add_height(margin().top() + margin().bottom());
		}

		return preferred_size;
	}

	bool HBox::UpdateTest (const UpdateRequest& request)
	{
		if(request.source() == Predefined) {

			switch (request.type()) {

				case SubWidgetSize:
					return false;	// DO not allow sub widget geometry reset outside

				case SubWidgetPosition:
					return false;

				default:
					return AbstractDequeContainer::UpdateTest(request);

			}

		} else {
			return false;
		}
	}

	void HBox::Update (const UpdateRequest& request)
	{
		if(request.source() == Predefined) {

			switch (request.type()) {

				case FormPosition: {
					const Point* new_pos = static_cast<const Point*>(request.data());
					int x = new_pos->x() - position().x();
					int y = new_pos->y() - position().y();

					MoveSubWidgets(x, y);

					break;
				}

				case FormSize: {
					const Size* size_p = static_cast<const Size*>(request.data());
					//if(sub_widget_size())
						//FillSubWidgetsInHBox(size_p, &margin(), space());

					break;
				}

				case ContainerMargin: {
					const Margin* margin_p = static_cast<const Margin*>(request.data());
					//if(sub_widget_size())
						//FillSubWidgetsInHBox(&size(), margin_p, space());
					break;
				}

				case LayoutPropertySpace: {
					const int* space_p = static_cast<const int*>(request.data());
					//if(sub_widget_size())
						//FillSubWidgetsInHBox(&size(), &margin(), *space_p);
					break;
				}

				case WidgetRefresh: {
					Refresh();
					break;
				}

				default: {
					break;
				}
			}

		}
	}

	ResponseType HBox::Draw (const RedrawEvent& event)
	{
		return IgnoreAndContinue;
	}

	ResponseType HBox::CursorEnterEvent (bool entered)
	{
		return IgnoreAndContinue;
	}

	ResponseType HBox::KeyPressEvent (const KeyEvent& event)
	{
		return IgnoreAndContinue;
	}

	ResponseType HBox::ContextMenuPressEvent (
	        const ContextMenuEvent& event)
	{
		return IgnoreAndContinue;
	}

	ResponseType HBox::ContextMenuReleaseEvent (
	        const ContextMenuEvent& event)
	{
		return IgnoreAndContinue;
	}

	ResponseType HBox::MousePressEvent (const MouseEvent& event)
	{
		return IgnoreAndContinue;
	}

	ResponseType HBox::MouseReleaseEvent (const MouseEvent& event)
	{
		return IgnoreAndContinue;
	}

	ResponseType HBox::MouseMoveEvent (const MouseEvent& event)
	{
		return IgnoreAndContinue;
	}

	void HBox::AddItem (AbstractWidget* object)
	{
		/*
		// don't fire events when adding a widget into a layout
		object->deactivate_events();
		deactivate_events();

		Size min_size = minimal_size();
		Size prefer_size = preferred_size();
		Size current_size = size();
		// TODO: count max size

		unsigned int h_plus = margin().top() + margin().bottom();

		if (sub_widget_size() == 0) {
			min_size.add_width(object->minimal_size().width());
			prefer_size.add_width(object->preferred_size().width());
		} else {
			min_size.add_width(object->minimal_size().width() + space());
			prefer_size.add_width(object->preferred_size().width() + space());
		}

		min_size.set_height(
		        std::max(min_size.height(), object->minimal_size().height() + h_plus));
		prefer_size.set_height(
		        std::max(prefer_size.height(),
		                object->preferred_size().height() + h_plus));

		if (current_size.width() < prefer_size.width()) {
			current_size.set_width(prefer_size.width());
		}
		if (current_size.height() < prefer_size.height()) {
			current_size.set_height(prefer_size.height());
		}

		SetPreferredSize(prefer_size);
		SetMinimalSize(min_size);

		if(object->expand_x()) {
			set_expand_x(true);
		}

		AppendSubWidget(object);

		if( !(current_size == size()) )
			Resize(current_size);
		else
			MakeLayout(&current_size, &margin(), space());

		activate_events();
		object->activate_events();
		*/
	}

	void HBox::RemoveItem (AbstractWidget* object)
	{
		deactivate_events();

		if(!RemoveSubWidget(object)) return;

		Size new_preferred_size;
		Size new_minimal_size;

		GetSizeHint(true, true, 0, &new_minimal_size, &new_preferred_size);

		//SetMinimalSize(new_minimal_size);
		//SetPreferredSize(new_preferred_size);

		set_expand_x(false);
		for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			if((*it)->expand_x()) {
				set_expand_x(true);
				break;
			}
		}

		//FillSubWidgetsInHBox(&size(), &margin(), space());

		activate_events();

		RemoveSubWidget(object);
	}

	void HBox::FillSubWidgetsInHBox (const Point& out_pos, const Size& out_size, const Margin& margin,
	        int space)
	{

		int x = out_pos.x() + margin.left();
		int y = out_pos.y() + margin.bottom();
		unsigned int width = out_size.width() - margin.left() - margin.right();
		unsigned int height = out_size.height() - margin.top() - margin.bottom();

		FillSubWidgetsProportionally(x, y, width, height, space);
	}

	void HBox::FillSubWidgetsInHBox (const Point& pos, const Size& size, int space)
	{
		FillSubWidgetsProportionally(pos.x(), pos.y(), size.width(), size.height(), space);
	}

	void HBox::FillSubWidgetsProportionally (int x, int y, unsigned int width,
					unsigned int height, int space)
	{
		boost::scoped_ptr<std::deque<Size> > expandable_prefers(new std::deque<Size>);
		boost::scoped_ptr<std::deque<Size> > unexpandable_prefers(new std::deque<Size>);

		unsigned int expandable_prefer_sum = 0;	// the width sum of the expandable widgets' size
		unsigned int unexpandable_prefer_sum = 0;	// the width sum of the unexpandable widgets' size

		Size tmp_size;
		for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			if ((*it)->visiable()) {
				tmp_size = (*it)->GetPreferredSize();

				if((*it)->expand_x()) {
					expandable_prefers->push_back(tmp_size);
					expandable_prefer_sum += tmp_size.width();
				} else {
					unexpandable_prefers->push_back(tmp_size);
					unexpandable_prefer_sum += tmp_size.width();
				}
			}
		}

		if((expandable_prefers->size() + unexpandable_prefers->size()) == 0) return;	// do nothing if all sub widgets are invisible

		unsigned int total_space = ((expandable_prefers->size() + unexpandable_prefers->size()) - 1) * space;

		unsigned int total_preferred_width = expandable_prefer_sum
						+ unexpandable_prefer_sum
						+ total_space;

		if (total_preferred_width == width) {
			DistributeWithPreferredWidth(position().x(), space,
							expandable_prefers.get(),
							unexpandable_prefers.get());
		} else if (total_preferred_width < width) {
			DistributeWithLargeWidth(x, width, space, expandable_prefers.get(),
							expandable_prefer_sum, unexpandable_prefers.get(),
							unexpandable_prefer_sum);
		} else {
			DistributeWithSmallWidth(x, width, space, expandable_prefers.get(),
							expandable_prefer_sum, unexpandable_prefers.get(),
							unexpandable_prefer_sum);
		}

		Align(y, height);
	}

	void HBox::DistributeWithPreferredWidth (int x, int space,
					const std::deque<Size>* expandable_prefers,
					const std::deque<Size>* unexpandable_prefers)
	{
		std::deque<Size>::const_iterator exp_it = expandable_prefers->begin();
		std::deque<Size>::const_iterator unexp_it = unexpandable_prefers->begin();

		WidgetDeque::iterator widget_it = sub_widgets()->begin();
		AbstractWidget* widget = 0;

		while (widget_it != sub_widgets()->end()) {

			widget = *widget_it;

			if(widget->visiable()) {

				if(widget->expand_x() && exp_it != expandable_prefers->end()) {
					ResizeSubWidget(widget, exp_it->width(), widget->size().height());
					SetSubWidgetPosition(widget, x, widget->position().y());
					exp_it++;
				} else {
					ResizeSubWidget(widget, unexp_it->width(), widget->size().height());
					SetSubWidgetPosition(widget, x, widget->position().y());
					unexp_it++;
				}

				x += widget->size().width() + space;
			}

			widget_it++;
		}
	}

	void HBox::DistributeWithSmallWidth (int x,
					unsigned int width,
					int space,
					const std::deque<Size>* expandable_prefers,
					unsigned int expandable_prefer_sum,
					const std::deque<Size>* unexpandable_prefers,
					unsigned int unexpandable_prefer_sum)
	{
		int widgets_width = width - (expandable_prefers->size() + unexpandable_prefers->size() - 1) * space;

		if(widgets_width <= 0) {
			for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
			{
				(*it)->Resize(0, (*it)->size().height());
			}
			return;
		}

		unsigned int reference_width;
		std::deque<Size>::const_iterator exp_it = expandable_prefers->begin();
		std::deque<Size>::const_iterator unexp_it = unexpandable_prefers->begin();
		WidgetDeque::iterator it = sub_widgets()->begin();
		AbstractWidget* widget = 0;

		if(widgets_width <= unexpandable_prefer_sum) {
			reference_width = unexpandable_prefer_sum;

			while (it != sub_widgets()->end()) {

				widget = (*it);

				if(widget->visiable()) {

					if (widget->expand_x() && exp_it != expandable_prefers->end()) {
						ResizeSubWidget(widget, 0, widget->size().height());
						SetSubWidgetPosition(widget, x, widget->position().y());
						exp_it++;
					} else {
						ResizeSubWidget(widget,
										reference_width * unexp_it->width()
														/ expandable_prefer_sum,
										widget->size().height());
						SetSubWidgetPosition(widget, x, widget->position().y());
						unexp_it++;
					}

					x += widget->size().width() + space;
				}

				it++;
			}

		} else {
			reference_width = widgets_width - unexpandable_prefer_sum;

			while (it != sub_widgets()->end()) {

				widget = (*it);

				if(widget->visiable()) {

					if (widget->expand_x() && exp_it != expandable_prefers->end()) {
						ResizeSubWidget(widget,
										reference_width * exp_it->width()
														/ expandable_prefer_sum,
										widget->size().height());
						SetSubWidgetPosition(widget, x, widget->position().y());
						exp_it++;
					} else {
						ResizeSubWidget(widget, unexp_it->width(), widget->size().height());
						SetSubWidgetPosition(widget, x, widget->position().y());
						unexp_it++;
					}

					x += widget->size().width() + space;
				}

				it++;
			}

		}




	}

	void HBox::DistributeWithLargeWidth (int x,
					unsigned int width,
					int space,
					const std::deque<Size>* expandable_prefers,
					unsigned int expandable_prefer_sum,
					const std::deque<Size>* unexpandable_prefers,
					unsigned int unexpandable_prefer_sum)
	{
		unsigned int widgets_width = width - (expandable_prefers->size() + unexpandable_prefers->size() - 1) * space;

		unsigned int expandable_width = widgets_width - unexpandable_prefer_sum;

		std::deque<Size>::const_iterator exp_it = expandable_prefers->begin();
		std::deque<Size>::const_iterator unexp_it = unexpandable_prefers->begin();

		WidgetDeque::iterator it = sub_widgets()->begin();

		AbstractWidget* widget = 0;
		while (it != sub_widgets()->end()) {

			widget = (*it);

			if(widget->visiable()) {

				if (widget->expand_x() && exp_it != expandable_prefers->end()) {
					ResizeSubWidget(widget,
									expandable_width * exp_it->width()
													/ expandable_prefer_sum,
									widget->size().height());
					SetSubWidgetPosition(widget, x, widget->position().y());
					exp_it++;
				} else {
					ResizeSubWidget(widget, unexp_it->width(), widget->size().height());
					SetSubWidgetPosition(widget, x, widget->position().y());
					unexp_it++;
				}

				x += widget->size().width() + space;
			}

			it++;
		}
	}

	void HBox::DistributeWithSmallWidth(int x, int space, const std::deque<Size>* list)
	{
		/*
		unsigned int min_expd_width = GetAllMinimalExpandableWidth();
		unsigned int fixed_width = GetAllFixedWidth();
		unsigned int current_width = size->width();
		unsigned int margin_plus = margin->left() + margin->right();

		WidgetDeque::iterator it;
		AbstractWidget* child = 0;

		bool change_expd_items = (current_width - margin_plus)
						>= (min_expd_width + fixed_width
										+ (sub_widget_size() - 1) * space);

		unsigned int exp_num = CountHExpandableNumber();

		if(change_expd_items) {

			// just change expandable widgets
			if (exp_num) {

				unsigned int average_expd_width = current_width - margin_plus
				        - fixed_width - (sub_widget_size() - 1) * space;
				average_expd_width = average_expd_width / exp_num;

				for (it = sub_widgets()->begin(); it != sub_widgets()->end(); it++) {
					child = *it;

					if (child->expand_x()) {
						ResizeSubWidget(child, average_expd_width, child->size().height());
					} else {
						ResizeSubWidget(child, child->preferred_size().width(),
						        child->size().height());
					}
				}

			}

		} else {

			unsigned int fixed_num = sub_widget_size() - exp_num;

			if(fixed_num) {

				std::list<AbstractWidget*> unminimized_items;
				unsigned int width_plus = 0;

				unsigned int total_fixed_width = current_width - margin_plus
				        - min_expd_width - (sub_widget_size() - 1) * space;
				unsigned int average_fixed_width = total_fixed_width / fixed_num;

				for (it = sub_widgets()->begin(); it != sub_widgets()->end(); it++) {

					child = *it;

					if (child->expand_x()) {
						ResizeSubWidget(child, child->minimal_size().width(),
						        child->size().height());
					} else {
						if (average_fixed_width
								< child->minimal_size().width()) {
							width_plus = width_plus + child->minimal_size().width() - average_fixed_width;
							ResizeSubWidget(child, child->minimal_size().width(),
									child->size().height());
						} else {
							unminimized_items.push_back(child);
							ResizeSubWidget(child, average_fixed_width,
									child->size().height());
						}
					}
				}

				if(width_plus > 0) {
					AdjustMinimalWidth(&unminimized_items, width_plus);
				}

			}
		}

		int x = position().x() + margin->left();	// the x position of each child widget, update in each for loop
		Distribute(space, x);

		*/
	}

	void HBox::DistributeWithLargeWidth(int x, int space, const std::deque<Size>* list)
	{
		/*
		unsigned int fixed_width = GetAllFixedWidth();
		unsigned int current_width = size->width();
		unsigned int margin_plus = margin->left() + margin->right();

		WidgetDeque::iterator it;
		AbstractWidget* child = 0;
		unsigned int exp_num = CountHExpandableNumber();

		if(exp_num) {

			unsigned int max_expd_width = GetAllMaximalExpandableWidth();
			unsigned int total_expd_width = current_width - margin_plus
			        - fixed_width - (sub_widget_size() - 1) * space;
			unsigned int average_expd_width = total_expd_width / exp_num;

			bool change_expd_items = (current_width - margin_plus)
			        <= (max_expd_width + fixed_width
			                + (sub_widget_size() - 1) * space);

			if(change_expd_items) {

				std::list<AbstractWidget*> unmaximized_list;
				unsigned int width_plus = 0;
				int x = position().x() + margin->left();

				for (it = sub_widgets()->begin(); it != sub_widgets()->end(); it++) {
					child = *it;
					if(child->expand_x()) {
						if(average_expd_width > child->maximal_size().width()) {
							width_plus = width_plus + average_expd_width - child->maximal_size().width();
							ResizeSubWidget(child, child->maximal_size().width(), child->size().height());
						} else {
							unmaximized_list.push_back(child);
							ResizeSubWidget(child, average_expd_width, child->size().height());
						}
					}
				}

				if(width_plus > 0) {	// check the last item
					AdjustExpandableWidth(&unmaximized_list, width_plus);
				}

				Distribute(space, x);

			} else {
				int x = position().x() + margin->left();

				x = x + (current_width - margin_plus - max_expd_width - fixed_width
		                - (sub_widget_size() - 1) * space) / 2;

				// resize all with the max size
				for(it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
				{
					child = *it;

					if (child->expand_x()) {
						ResizeSubWidget(child, child->maximal_size().width(), child->size().height());
					} else {
						ResizeSubWidget(child, child->preferred_size().width(),
						        child->size().height());
					}
				}

				Distribute(space, x);
			}

		} else {
			int x = position().x() + margin->left();

			// if no expandable items, center all items
			x = x + (current_width - margin_plus - fixed_width - (sub_widget_size() - exp_num - 1) * space) / 2;

			// resize all with preferred width
			for(it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
			{
				child = *it;
				ResizeSubWidget(child, child->preferred_size().width(),
					        child->size().height());
			}

			Distribute(space, x);
		}
	*/
	}

	void HBox::Distribute(int space, int start)
	{
		start -= space;	// subtract one space to make sure no space if only 1 child in layout
		for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			start += space;

			SetSubWidgetPosition(*it, start, (*it)->position().y());
			start += (*it)->size().width();
		}
	}

	void HBox::Align(int y, unsigned int height)
	{
		WidgetDeque::iterator it;
		AbstractWidget* widget = 0;
		for(it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			widget = *it;

			if (widget->expand_y() ||
					(widget->size().height() > height)) {
				ResizeSubWidget(widget, widget->size().width(), height);
				SetSubWidgetPosition(widget, widget->position().x(), y);
			} else {

				if (alignment() & AlignTop) {
					SetSubWidgetPosition(widget, widget->position().x(),
					        y + (height - widget->size().height()));
				} else if (alignment() & AlignBottom) {
					SetSubWidgetPosition(widget, widget->position().x(), y);
				} else if (alignment() & AlignHorizontalCenter) {
					SetSubWidgetPosition(widget, widget->position().x(),
					        y + (height - widget->size().height()) / 2);
				}

			}
		}
	}

	void HBox::Align(const Size* size, const Margin* margin)
	{
		int y = position().y() + margin->bottom();

		unsigned int h = size->height() - margin->top() - margin->bottom();

		WidgetDeque::iterator it;
		AbstractWidget* child = 0;
		for(it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			child = *it;

			if (child->expand_y() ||
					(child->size().height() > h)) {
				ResizeSubWidget(child, child->size().width(), h);
				SetSubWidgetPosition(child, child->position().x(), y);
			} else {

				if (alignment() & AlignTop) {
					SetSubWidgetPosition(child, child->position().x(),
					        y + (h - child->size().height()));
				} else if (alignment() & AlignBottom) {
					SetSubWidgetPosition(child, child->position().x(), y);
				} else if (alignment() & AlignHorizontalCenter) {
					SetSubWidgetPosition(child, child->position().x(),
					        y + (h - child->size().height()) / 2);
				}

			}
		}
	}

	unsigned int HBox::AdjustExpandableWidth(std::list<AbstractWidget*>* item_list_p, unsigned int width_plus)
	{
		if(!item_list_p) return width_plus;
		if(item_list_p->size() == 0) return width_plus;

		unsigned int remainder = 0;
		std::list<AbstractWidget*>::iterator it;
		unsigned int average_width_plus = width_plus / item_list_p->size();

		for(it = item_list_p->begin(); it != item_list_p->end(); it++)
		{
			/*
			if ((average_width_plus + (*it)->size().width()) > (*it)->maximal_size().width()) {
				ResizeSubWidget(*it, (*it)->maximal_size().width(), (*it)->size().height());
				remainder = remainder + average_width_plus + (*it)->size().width() - (*it)->maximal_size().width();
				it = item_list_p->erase(it);
			} else {
				ResizeSubWidget(*it, (*it)->size().width() + average_width_plus, (*it)->size().height());
			}
			*/
		}

		if(remainder != 0) {
			// TODO: do not use iteration procedure
			remainder = AdjustExpandableWidth(item_list_p, remainder);
		}

		return remainder;
	}

	unsigned int HBox::AdjustMinimalWidth(std::list<AbstractWidget*>* item_list_p, unsigned int width_plus)
	{
		if(!item_list_p) return width_plus;
		if(item_list_p->size() == 0) return width_plus;

		unsigned int remainder = 0;
		std::list<AbstractWidget*>::iterator it;
		unsigned int average_width_plus = width_plus / item_list_p->size();

		for(it = item_list_p->begin(); it != item_list_p->end(); it++)
		{
			/*
			if (((*it)->size().width() - average_width_plus) < (*it)->minimal_size().width()) {
				ResizeSubWidget(*it, (*it)->minimal_size().width(), (*it)->size().height());
				remainder = remainder + (*it)->minimal_size().width() - ((*it)->size().width() - average_width_plus);
				it = item_list_p->erase(it);
			} else {
				ResizeSubWidget(*it, (*it)->size().width() - average_width_plus, (*it)->size().height());
			}
			*/
		}

		if(remainder != 0) {
			// TODO: do not use iteration procedure
			remainder = AdjustMinimalWidth(item_list_p, remainder);
		}

		return remainder;
	}

	unsigned int HBox::GetAllMinimalExpandableWidth()
	{
		unsigned int width = 0;

		/*
		for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			if((*it)->expand_x())
				width += (*it)->minimal_size().width();
		}
		*/

		return width;
	}

	unsigned int HBox::GetAllMaximalExpandableWidth()
	{
		unsigned int width = 0;

		/*
		for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			if((*it)->expand_x())
				width += (*it)->maximal_size().width();
		}
		*/

		return width;
	}

	unsigned int HBox::GetAllFixedWidth()
	{
		unsigned int width = 0;

		for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			if(!(*it)->expand_x())
				width += (*it)->size().width();
		}

		return width;
	}
	
	unsigned int HBox::CountHExpandableNumber ()
	{
		unsigned int num = 0;

		for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			if((*it)->expand_x()) {
				num++;
			}
		}

		return num;
	}
	
	void HBox::GetSizeHint (bool count_margin,
										  bool count_space,
										  Size* size,
										  Size* min,
										  Size* preferred)
	{
		/*
		Size size_out;
		Size min_size_out;
		Size preferred_size_out;

		AbstractWidget* child;
		WidgetDeque::iterator it;

		if(count_margin) {
			size_out.set_width(margin().left());
			min_size_out.set_width(margin().left());
			preferred_size_out.set_width(margin().left());
		}

		for(it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			child = *it;
			size_out.add_width(child->size().width());
			size_out.set_height(std::max(size_out.height(),
					child->size().height()));
			min_size_out.add_width(child->minimal_size().width());
			min_size_out.set_height(std::max(min_size_out.height(),
					child->minimal_size().height()));
			preferred_size_out.add_width(child->preferred_size().width());
			preferred_size_out.set_height(std::max(preferred_size_out.height(),
					child->size().height()));
		}

		if(count_margin) {
			size_out.add_width(margin().right());
			size_out.add_height(margin().top() + margin().bottom());

			min_size_out.add_width(margin().right());
			min_size_out.add_height(margin().top() + margin().bottom());

			preferred_size_out.add_width(margin().right());
			preferred_size_out.add_height(margin().top() + margin().bottom());
		}

		if(count_space) {
			if(sub_widget_size()) {
				size_out.add_width((sub_widget_size() - 1) * space());
				min_size_out.add_width((sub_widget_size() - 1) * space());
				preferred_size_out.add_width((sub_widget_size() - 1) * space());
			}
		}

		if(size) *size = size_out;
		if(min) *min = min_size_out;
		if(preferred) *preferred = preferred_size_out;
		*/
	}

}
