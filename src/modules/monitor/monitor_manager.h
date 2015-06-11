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


#ifndef MONITOR_MANAGER_H
#define MONITOR_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "../../module.h"
#include "../connection/connection_manager.h"

namespace amods {

	class Factory;

	namespace monitor {
		struct System {
			std::string address;
			size_t num;
			unsigned int timeout_ms;
		};
		struct Response {
			size_t num;
			float min;
			float max;
			float avg;
			std::vector<float> times;
			std::vector< std::map<std::string, std::string> > data;
			std::vector< std::pair<unsigned int, std::string> > error; // errno -> errmsg
		};

		class Monitor
		{
			protected:
				Factory *module_factory;
				std::string moduleName;
				std::string moduleDescription;
				System system;
				std::vector< std::pair<std::string, std::string> > send_data;
				std::vector< std::pair<std::string, std::string> >::iterator it_send_data;

			public:                     // see http://www.daniweb.com/software-development/cpp/threads/114299
				virtual ~Monitor() {};    // to prevent "undefined symbols" and "undefined reference to vtable of..." use {} here
				virtual Monitor* GetInstance(Factory *factory) = 0;
				virtual const std::string &GetName() { return moduleName; };
				virtual const std::string &GetDescription() { return moduleDescription; };
				virtual void SetSystem(System sys) { system = sys; };
				virtual System GetSystem() {
					if (!system.num || system.num <= 0) { system.num = 1; }
					if (!system.timeout_ms || system.timeout_ms <= 0) { system.timeout_ms = 1000; }
					return system;
				};
				virtual void SetData(std::string data) { send_data.push_back(std::make_pair<std::string, std::string>("", data)); };
				virtual void SetData(std::string key, std::string value) { send_data.push_back(std::make_pair<std::string, std::string>(key, value)); };
				virtual void SetData(std::vector< std::pair<std::string, std::string> > data) { send_data = data; };
				virtual Response BeginMonitor() = 0;
		};

		class MonitorManager
		{
			public:
				~MonitorManager() {
					if (!modulesVector.empty()) {
						for (ModuleVector::reverse_iterator it = modulesVector.rbegin(); it != modulesVector.rend(); it++) {
							//delete *it;
						}
						modulesVector.clear();
					}
				};

				void addModule(std::auto_ptr<Monitor> module) {
					modulesVector.push_back(module.release());
				};

				size_t GetModulesCount() const {
					return modulesVector.size();
				};

				Monitor *GetModule(size_t index) {
					if (index > GetModulesCount() || index < 0) return NULL;
					return modulesVector.at(index);
				};

			private:
				typedef std::vector<Monitor *> ModuleVector;
				ModuleVector modulesVector;
		};
	}
}
#endif // MONITOR_MANAGER_H
