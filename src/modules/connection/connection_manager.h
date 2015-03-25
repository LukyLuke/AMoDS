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

#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "../../module.h"

namespace amods {

	class Factory;

	namespace connections {
		struct Request {
			std::string host;
			unsigned int port;
			unsigned char response_data:1;
			std::string data;
		};
		struct Response {
			std::string data;
			std::string error;
			int errnum;
			std::string errmsg;
		};
		
		class Connection
		{
			protected:
				Factory *module_factory;
				std::string moduleName;
				std::string moduleDescription;
				
			public:                     // see http://www.daniweb.com/software-development/cpp/threads/114299
				virtual ~Connection() {}; // to prevent "undefined symbols" and "undefined reference to vtable of..." use {} here
				virtual Connection* GetInstance(Factory *factory) = 0;
				virtual const std::string &GetName() { return moduleName; };
				virtual const std::string &GetDescription() { return moduleDescription; };
				virtual void SendRequest(Request req) = 0;
				virtual Response GetResponse() = 0;
		};

		class ConnectionManager
		{
			public:
				~ConnectionManager() {
					if (!modulesVector.empty()) {
						for (ModuleVector::reverse_iterator it = modulesVector.rbegin(); it != modulesVector.rend(); it++) {
							//delete *it;
						}
						modulesVector.clear();
					}
				};
				
				void addModule(std::auto_ptr<Connection> connection) {
					modulesVector.push_back(connection.release());
				};
				
				size_t GetModulesCount() const {
					return modulesVector.size();
				};
				
				Connection *GetModule(size_t index) {
					if (index > GetModulesCount() || index < 0) return NULL;
					return modulesVector.at(index);
				};
				
			private:
				typedef std::vector<Connection *> ModuleVector;
				ModuleVector modulesVector;
		};
	}
}
#endif // CONNECTION_MANAGER_H
