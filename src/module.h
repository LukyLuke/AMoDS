/*
 * < one line to gi*ve the program's name and a brief idea of what it does.>
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

#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <map>

namespace amods {
	class Factory;

	const static unsigned int ENGINE_VERSION = 1;
	
	class Module
	{
		private:
			typedef void fnRegisterPlugin(Factory &);
			typedef int fnGetEngineVersion();
			fnRegisterPlugin *f_fnRegisterPlugin;
			fnGetEngineVersion *f_fnGetEngineVersion;
			int ref_counter;

		protected:
			std::string file;
			void *handle;
			bool loaded;

		public:
			Module(const std::string &fileName);
			virtual ~Module();
			void FreeModule();
			std::string &GetFileName() { return file; };
			int GetEngineVersion() const {
				if (loaded) return f_fnGetEngineVersion();
				return -1;
			};
			void RegisterPlugin(Factory &factory) {
				if (loaded) f_fnRegisterPlugin(factory);
			};
			bool isLoaded() { return loaded; };
	};
}

#endif // MODULE_H