/*
 * The main ThreadClass
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


#ifndef THREAD_H
#define THREAD_H
#include <pthread.h>

namespace amods {

	class Thread
	{
		private:
			pthread_t p_thread;
			pthread_attr_t p_attr;
			void * _arg;

		protected:
			int Run(void* arg);
			static void* EntryPoint(void*);
			virtual void Setup();
			virtual void Execute(void*);
			void* Arg() const { return _arg; }
			void Arg(void* arg) { _arg = arg; }

		public:
			Thread();
			virtual ~Thread();
			int Start(void* arg);
			int WaitUntilEnd();
	};
}
#endif // AMODS_H
