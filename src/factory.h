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


#ifndef FACTORY_H
#define FACTORY_H

#include <string>
#include <map>

#include "module.h"
#include "modules/connection/connection_manager.h"
#include "modules/monitor/monitor_manager.h"

namespace amods {
	const unsigned int MODULE_ENGINE_VERSION = 1;
	
	class Factory
	{
		private:
			typedef std::map< std::string, Module*> ModulesMap;
			ModulesMap loadedModules;
			connections::ConnectionManager connections;
			monitor::MonitorManager monitors;
			
		public:
			Factory();
			virtual ~Factory();
			void LoadPlugin(const std::string &fileName);
			
			connections::ConnectionManager &GetConnectionsManager() { return connections; };
			connections::Connection *getConnection(std::string name);
			
			monitor::MonitorManager &GetMonitorManager() { return monitors; };
			monitor::Monitor *getMonitor(std::string name);
	};

}
#endif // FACTORY_H
