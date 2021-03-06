/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2011  Lukas Zurschmiede <l.zurschmiede@delightsoftware.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstdio>
#include "guid.h"
#include "random.h"

namespace lukyluke {
	namespace helper {

		std::string GetGuid() {
			int len = 33;
			char buffer[len];
			buffer[len] = '\0';
			std::snprintf(buffer, len, "%04x%04x-%04x-%03x4-%04x-%04x%04x%04x",
				RandomInt(0, 65535), RandomInt(0, 65535),  // 32 Bits for "time_low"
				RandomInt(0, 65535),                       // 16 Bits for "time_mid"
				RandomInt(0, 4095),                        // 12 Bits before the 0100 of (version) 4 for "time_hi_and_version"
				RandomInt(0, 65535),   // this here we don't implement
				// 8 bits, the last two of which (positions 6 and 7) are 01, for "clk_seq_hi_res"
				// (hence, the 2nd hex digit after the 3rd hyphen can only be 1, 5, 9 or d)
				// 8 bits for "clk_seq_low"
				RandomInt(0, 65535), RandomInt(0, 65535), RandomInt(0, 65535) // 48 Bits for "node"
			);
			return std::string(buffer);
		}
	}
}
