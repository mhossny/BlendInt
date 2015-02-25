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

#include <string>
#include <bitset>

#include <Cpp/Events.hpp>

#include <core/margin.hpp>

#include <gui/abstract-icon.hpp>
#include <gui/text.hpp>
#include <gui/abstract-round-widget.hpp>

namespace BlendInt {

	class ButtonGroup;

	class AbstractButton: public AbstractRoundWidget
	{
		DISALLOW_COPY_AND_ASSIGN(AbstractButton);

	public:

		AbstractButton ();

		AbstractButton (const String& text);

		AbstractButton (const RefPtr<AbstractIcon>& icon);

		AbstractButton (const RefPtr<AbstractIcon>& icon, const String& text);

		virtual ~AbstractButton ();

		void SetDown (bool down);

		void SetCheckable (bool checkabel);

		void SetChecked (bool checked);

		virtual Size GetPreferredSize () const;

		const RefPtr<AbstractIcon>& icon () const
		{
			return icon_;
		}

		const RefPtr<Text>& text () const
		{
			return text_;
		}

		bool hover() const {return m_status[ButtonHover];}

		bool is_down () const {return m_status[ButtonDown];}

		bool is_checked () const {return m_status[ButtonChecked];}

		bool is_checkable () const {return m_status[ButtonCheckable];}

		bool is_pressed () const {return m_status[ButtonPressed];}

		Cpp::EventRef<AbstractButton*> clicked() {return clicked_;}

		Cpp::EventRef<AbstractButton*, bool> toggled() {return toggled_;}

		Cpp::EventRef<AbstractButton*> pressed () {return pressed_;}

		static Margin kPadding;

		static int kIconTextSpace;	// the space between icon and text

	protected:

		virtual Response Draw (AbstractWindow* context) = 0;

		virtual void PerformHoverIn (AbstractWindow* context);

		virtual void PerformHoverOut (AbstractWindow* context);

		virtual Response PerformMousePress (AbstractWindow* context);

		virtual Response PerformMouseRelease (AbstractWindow* context);

		virtual Response PerformMouseMove (AbstractWindow* context);

		inline void set_icon (const RefPtr<AbstractIcon>& icon)
		{
			icon_ = icon;
		}

		inline void set_text (const String& text)
		{
			if(text_) {
				text_->SetText(text);
			} else {
				text_.reset(new Text(text));
			}
		}

		inline void set_font (const Font& font)
		{
			if(text_) text_->SetFont(font);
		}

		void DrawIconText ();

		void set_down (bool down)
		{
			m_status[ButtonDown] = down ? 1 : 0;
		}

		void set_checkable (bool checkable)
		{
			m_status[ButtonCheckable] = checkable ? 1 : 0;
		}

		void set_checked (bool checked)
		{
			m_status[ButtonChecked] = checked ? 1 : 0;
		}

		void set_text (const RefPtr<Text>& text)
		{
			text_ = text;
		}

	private:

		friend class ButtonGroup;

		enum ButtonStatusIndex {
			ButtonPressed = 0,
			ButtonHover,
			ButtonDown,
			ButtonCheckable,
			ButtonChecked,
			ButtonLastChecked
		};

		RefPtr<AbstractIcon> icon_;

		RefPtr<Text> text_;

		std::bitset<8> m_status;

		/**
		 * @brief press event
		 */
		Cpp::Event<AbstractButton*> pressed_;

		/**
		 * @brief release event
		 */
		Cpp::Event<AbstractButton*> released_;

		/**
		 * @brief click event
		 *
		 * Mouse press and release in the button causes a clicked event.
		 */
		Cpp::Event<AbstractButton*> clicked_;

		Cpp::Event<AbstractButton*, bool> toggled_;

		ButtonGroup* group_;
	};

} /* namespace BIL */
