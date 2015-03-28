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

#include <ctime>
#include <cstdlib>
#include "random.h"

namespace lukyluke {
	namespace helper {
		bool __random_seed_initialized = false;
		
		void RandomSeedInit() {
			srand((unsigned)std::time(0));
		}

		int RandomInt(int min, int max) {
			RandomSeedInit();
			if (max <= min) {
				return int((max == min) ? min : max);
			}
			int range;
			range = int(max - min + 1);
			return (min + int( range * (rand()/(RAND_MAX+1.0)) ));
		}
	}
}
