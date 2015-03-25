/*
		<one line to give the program's name and a brief idea of what it does.>
		Copyright (C) 2011  Lukas Zurschmiede <l.zurschmiede@delightsoftware.com>

		This program is free software: you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation, either version 3 of the License, or
		(at your option) any later version.

		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License
		along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <map>
#include <string>
#include <iostream>

#include "factory.h"
#include "module.h"

namespace amods {
	
	Factory::Factory() {}

	Factory::~Factory() {
		for (ModulesMap::reverse_iterator it = loadedModules.rbegin(); it != loadedModules.rend(); it++) {
			it->second->FreeModule();
			delete (it->second);
		}
		loadedModules.clear();
	}
	
	void Factory::LoadPlugin(const std::string &fileName) {
		// Load the Plugin only if it's not loaded already
		if (loadedModules.find(fileName) == loadedModules.end()) {
			Module* mod = new Module(fileName);
			if (mod->isLoaded() && (mod->GetEngineVersion() <= MODULE_ENGINE_VERSION)) {
				loadedModules.insert( ModulesMap::value_type(fileName, mod) ).first->second->RegisterPlugin(*this);
			} else {
				delete mod;
			}
		}
	}
	
	connections::Connection *Factory::getConnection(std::string name) {
		connections::ConnectionManager cm = GetConnectionsManager();
		connections::Connection *con;
		size_t idx;
		for (idx = 0; idx < cm.GetModulesCount(); idx++) {
			con = cm.GetModule(idx);
			if ((con != NULL) && (con->GetName().compare(name) == 0)) {
				return con->GetInstance(this);
			}
		}
		return NULL;
	}
	
	monitor::Monitor *Factory::getMonitor(std::string name) {
		monitor::MonitorManager cm = GetMonitorManager();
		monitor::Monitor *con;
		size_t idx;
		for (idx = 0; idx < cm.GetModulesCount(); idx++) {
			con = cm.GetModule(idx);
			if ((con != NULL) && (con->GetName().compare(name) == 0)) {
				return con->GetInstance(this);
			}
		}
		return NULL;
	}
		
}
