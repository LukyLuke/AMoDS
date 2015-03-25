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
			float times[];
			std::string data[];
			std::string header[];
		};

		class Monitor
		{
			protected:
				Factory *module_factory;
				std::string moduleName;
				std::string moduleDescription;
				System system;
				std::string send_data;
				std::vector<std::string> send_header;

			public:                     // see http://www.daniweb.com/software-development/cpp/threads/114299
				virtual ~Monitor() {};    // to prevent "undefined symbols" and "undefined reference to vtable of..." use {} here
				virtual Monitor* GetInstance(Factory *factory) = 0;
				virtual const std::string &GetName() { return moduleName; };
				virtual const std::string &GetDescription() { return moduleDescription; };
				virtual void SetSystem(System sys) { system = sys; };
				virtual System GetSystem() { return system; };
				virtual void SetData(std::string data) { send_data = data; };
				virtual void SetHeader(std::vector<std::string> header) { send_header = header; };
				virtual Response BeginMonitor() = 0;
				virtual Response BeginMonitor(std::string data) { SetData(data); return BeginMonitor(); };
				virtual Response BeginMonitor(std::string data, std::vector<std::string> header) { SetData(data); SetHeader(header); return BeginMonitor(); };
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
