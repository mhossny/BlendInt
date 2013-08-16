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

#ifndef _BIL_UTILITIES_H_
#define _BIL_UTILITIES_H_

namespace BIL {

	/**
	 * @brief Correct a numeric value in a scope
	 * @param[in] value the value to be corrected
	 * @param[in] min minimum of the scope
	 * @param[in] max maximum of the scope
	 * @return min if value < min, max if value > max,
	 * otherwize return value
	 */
	template <typename T>
	inline T correct_in_scope (T value, T min, T max)
	{
		return ((value) < (min) ?
				(min) : ((value) > (max) ?
						 (max) : (value)));
	}

	/**
	 * @brief Copy elements in arrary from src to dist
	 */
	template <typename T>
	inline void copy_arrary_v2 (T src[2], T dist[2])
	{
		dist[0] = src[0];
		dist[1] = src[1];
	}

}

#endif	// _BIL_UTILITIES_H_
