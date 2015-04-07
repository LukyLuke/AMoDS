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
#include <string.h>
#include <stdint.h>

#include "../../module.h"

namespace amods {

	class Factory;

	namespace connections {
		struct Request {
			std::string host;
			unsigned int port;
			unsigned char response_data:1;
		};
		struct Response {
			char *data;
			unsigned int data_length;
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
				char *data;
				unsigned int data_length;
				
			public:                     // see http://www.daniweb.com/software-development/cpp/threads/114299
				virtual ~Connection() {   // to prevent "undefined symbols" and "undefined reference to vtable of..." use {} here
					if (data != '\0') {
						delete[] data;
					}
				};
				virtual Connection* GetInstance(Factory *factory) = 0;
				virtual const std::string &GetName() { return moduleName; };
				virtual const std::string &GetDescription() { return moduleDescription; };
				virtual void SendRequest(Request req) = 0;
				virtual Response GetResponse() = 0;
				virtual void setData(char *_data, unsigned int _length) {
					if (_length > 0) {
						data_length = _length + 1;
						data = new char[_length + 1];
						memcpy(data, _data, _length);
						data[_length + 1] = '\0';
					}
				};
				
				static uint16_t ReverseBits(uint16_t num) {
					unsigned int count = sizeof(num) * 8;
					uint16_t reversed = 0, i;
					for (i = 0; i < count; i++) {
						// Check if bit at pos 'i' is set
						if (num & (1 << i)) {
							reversed |= (1 << (count - 1 - i)); // Set the bit at the reverse position
						}
					}
					return reversed;
				};
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
