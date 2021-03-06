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

#include "math.h"

namespace lukyluke {
	namespace helper {

		uint64_t factorial(uint32_t num) {
			if (num <= 0) return 1;
			if (num <= 2) return num;
			uint64_t fact = 1;
			while (num > 0) {
				fact *= num--;
			}
			return fact;
		}

		uint64_t binomial(uint32_t n, uint32_t k) {
			if (n <= 0 || k <= 0) return 1;
			//return factorial(n) / ( factorial(n-k) * factorial(k) );
			// This method uses much less memory but a little more calls
			if (k == 0) return 1;
			if (k == 1) return n;
			if (2*k > n) return binomial(n, n-k);
			
			uint64_t result = 1;
			uint32_t i;
			for (i = 1; i <= k; i++) {
				result = (result * (n - k + i) ) / i;
			}
			return result;
		}
	}
}
