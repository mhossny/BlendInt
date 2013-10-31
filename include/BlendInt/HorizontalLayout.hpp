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

#ifndef _BLENDINT_HORIZONTALLAYOUT_HPP_
#define _BLENDINT_HORIZONTALLAYOUT_HPP_

#include <BlendInt/AbstractLayout.hpp>

namespace BlendInt {

	class HorizontalLayout: public AbstractLayout
	{
		DISALLOW_COPY_AND_ASSIGN(HorizontalLayout);

	public:

		HorizontalLayout(int align = AlignHorizontalCenter);

		HorizontalLayout(int align, AbstractForm* parent);

		virtual ~HorizontalLayout ();

	protected:

		virtual void update (int property_type);

		//virtual bool update (int type, const void* property);

		virtual void render ();

		virtual void press_key (KeyEvent* event);

		virtual void press_context_menu (ContextMenuEvent* event);

		virtual void release_context_menu (ContextMenuEvent* event);

		virtual void press_mouse (MouseEvent* event);

		virtual void release_mouse (MouseEvent* event);

		virtual void move_mouse (MouseEvent* event);

		virtual void add_item (Widget* widget);

		virtual void add_item (AbstractLayout* layout);

		virtual void remove_item (AbstractForm* object);

	private:

		void make_layout ();

		/**
		 * @brief distribute horizontally with preferred size
		 */
		void distribute_with_preferred_size ();

		/**
		 * @brief distribute horizontally with small size
		 */
		void distribute_with_small_size ();

		/**
		 * @brief distribute horizontally with large size
		 */
		void distribute_with_large_size ();

		/**
		 * @brief align horizontally with preferred size
		 */
		void align_with_preferred_size ();

		/**
		 * @brief align horizontally
		 */
		void align ();

		/**
		 * @brief reset the width of unexpandable items
		 * @param[in] items
		 * @param[in] width
		 */
		void reset_width_of_fixed_items (std::set<AbstractForm*>* items, unsigned int width);

		unsigned int get_minimal_width_of_xexpandable_items ();

		unsigned int get_width_of_xunexpandable_items ();

		void change_layout (const Size* size);

		void generate_default_layout ();

		/**
		 * @brief scan the children and get the total size hint
		 * @param[in] if count margin
		 * @param[in] if count space
		 * @param[out] size the layout size
		 * @param[out] min the layout minimal size
		 * @param[out] prefer the layout preferred size
		 */
		void get_size_hint (bool margin, bool space, Size* size, Size* min, Size* prefer);

		Size recount_size ();

		/**
		 *@align the objects in the layout according to the alignment
		 * @param height[in] the max height of area contains children (height - (top + bottom margin))
		 */
		void align_along_x (unsigned int height);

		/**
		 * @brief set of x expandable items
		 */
		std::set<AbstractForm*> m_xexpandable_items;

		/**
		 * @brief set of x unexpandable items
		 */
		std::set<AbstractForm*> m_xunexpandable_items;
	};

}

#endif	// _BIL_HORIZONTALLAYOUT_H_
