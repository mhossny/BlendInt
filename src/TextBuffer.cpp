/*
 * This file is part of BIL (Blender Interface Library).
 *
 * BIL (Blender Interface Library) is free software: you can
 * redistribute it and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * BIL (Blender Interface Library) is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BIL.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Contributor(s): Freeman Zhang <zhanggyb@gmail.com>
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <algorithm>

#include <BIL/FontEngine.hpp>
#include <BIL/FontConfig.hpp>
#include <BIL/TextBuffer.hpp>

#include <BIL/Tuple.hpp>

using namespace std;

namespace BIL {

	/*
	TextBuffer::TextBuffer (const Font& font)
		: rowspacing_(1.0),
		  foreground_(0xFFFFFFFF),
		  background_(0x00000000)
	{
		set_font(font);
		//glShadeModel(GL_FLAT);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}
	*/

	TextBuffer::TextBuffer (const String& text, const Font& font)
		: rowspacing_(1.0), foreground_(0xFFFFFFFF), background_(0x00000000)
	{
		set_font(font);
		Append(text);

		//glShadeModel(GL_FLAT);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	TextBuffer::~TextBuffer ()
	{
	}

	void TextBuffer::Append (const String& text)
	{
		if (text.empty())
			return;

		text_.append(text);
	}

	void TextBuffer::Append (wchar_t charcode)
	{
		text_.push_back(charcode);
	}

	Size TextBuffer::CalculateOutlineBoxSize ()
	{
		String::const_iterator it;
		String::const_iterator next;
		Size outline_box;
		Tuple2l kerning;
		Glyph* glyph = NULL;

		unsigned int line_width = 0;
		unsigned int line = 1;

		int line_height = 0;
		int total_height = 0;
		for (it = text_.begin(); it != text_.end(); it++) {

			if (*it == '\n') {
				line++;
				outline_box.set_width (outline_box.width() > line_width ? outline_box.width() : line_width);
				total_height = total_height + line_height +
						static_cast<int>(fontcache_->getHeight() * (line -1) * (rowspacing_ - 1.0));
				continue;
			}

			glyph = fontcache_->query(*it);
			if (glyph) {

				// add kerning support
				next = it + 1;
				if(next != text_.end()) {
					kerning = fontcache_->getKerning(*it, *next);
				}
				line_width = glyph->metrics().horiAdvance + kerning.vec.x + line_width;

				Rect rect = glyph->OutlineBox();
				line_height = std::max (rect.height(), line_height);
			}
		}

		outline_box.set_width (outline_box.width() > line_width ? outline_box.width() : line_width);
		//outline_box.set_height (static_cast<unsigned int>(fontcache_->getHeight() * line + (line - 1) * rowspacing_));
		outline_box.set_height (total_height + line_height);

		return outline_box;
	}

	void TextBuffer::Render ()
	{
		Glyph* glyph = NULL;

		//glDisable(GL_LIGHTING);
		//glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4ub(foreground_.r(), foreground_.g(), foreground_.b(), foreground_.a());

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glTranslatef(origin_.x(),
					 origin_.y(), // - fontcache_->getDescender(),
					 origin_.z());


		int line = 0;
		String::const_iterator it;
		for (it = text_.begin(); it != text_.end(); it++) {
			if (*it == '\n') {
				line++;
				glLoadIdentity();
				glTranslatef(origin_.x(),
							 origin_.y()
							 - rowspacing_ * fontcache_->getHeight() * line,
							 origin_.z());
				continue;
			}

			glyph = fontcache_->query(*it);
			if (glyph) {
				glyph->Render();
				glTranslatef(glyph->metrics().horiAdvance, 0, 0);
			}
		}

		glPopMatrix();
		glDisable(GL_BLEND);

	}

	void TextBuffer::Clear (void)
	{
		text_.clear();
	}

} /* namespace BIL */
