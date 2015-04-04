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
		
		See RFC 1035: http://www.ietf.org/rfc/rfc1035.txt
*/

#include <bitset>
#include "dns.h"

namespace amods {
	namespace monitor {
		Dns::Dns(Factory *factory) {
			moduleName = "dns";
			moduleDescription = "Check a DNS Server for a defined record or for a functionality";
			module_factory = factory;
			_create_types();
			_create_classes();
		}
		
		Monitor* Dns::GetInstance(Factory *factory) {
			return new Dns(factory);
		}

		Dns::~Dns() {
			
		}

		Response Dns::BeginMonitor() {
			Response resp;
			resp.num = GetSystem().num;
			resp.min = 0.0;
			resp.max = 0.0;
			resp.avg = 0.0;
			resp.times[GetSystem().num];
			resp.data[GetSystem().num];
			
			SendRequest(&resp);
			return resp;
		}

		/**
		* Check the DNS Server.
		* @param Response *resp The reponse object with statistical data
		*/
		void Dns::SendRequest(Response *resp) {
			std::string _key, domain;
			uint8_t type = 1, _class = 1;
			uint16_t header_size = sizeof(struct DnsHeader);
			char dns_package[header_size];
			struct DnsHeader *header = (struct DnsHeader *)&dns_package;
			memset(header, 0, header_size);
			
			// Add a random UID and mark it as recursive.
			lukyluke::helper::RandomSeedInit();
			header->uid = (uint16_t)( 65535 * (rand() / (RAND_MAX + 1.0)) );
			header->type = 0;
			header->recursive = 1;
			header->num_questions = 1;
			
			// Get the domain and append it
			for (it_send_data = send_data.begin(); it_send_data != send_data.end(); it_send_data++) {
				_key = it_send_data->first;
				std::transform(_key.begin(), _key.end(), _key.begin(), ::tolower);
				if (_key == "domain") {
					domain = it_send_data->second;
					std::transform(domain.begin(), domain.end(), domain.begin(), ::tolower);
				}
				if (_key == "type") {
					type = getType(it_send_data->second);
				}
				if (_key == "class") {
					_class = getClass(it_send_data->second);
				}
			}
			
			// The domain is split out by the '.' and these are replaced by the length of the following domain part
			std::vector<std::string> data;
			std::size_t _pos = domain.find('.');
			uint16_t _length;
			while (_pos != std::string::npos) {
				data.push_back(domain.substr(0, _pos));
				_length += data.back().length() + 1; // +1 because there is always one byte which gives the length of the following domain part
				domain.erase(0, _pos + 1);
				_pos = domain.find('.');
			}
			
			// Calculate the size of the final char array we want to submit and copy the DNS-Package header
			char cdata[header_size + _length + 3]; // +3 for the type and class as last arguments and NULL termination
			memcpy(&cdata[0], &dns_package, header_size);
			
			// Attach each domain part, prefixed with the number of chars
			_length = header_size;
			for (std::vector<std::string>::iterator it = data.begin(); it != data.end(); it++) {
				unsigned short sl = (*it).size();
				memcpy(&cdata[_length++], (char *)&sl, 1);
				memcpy(&cdata[_length], (*it).c_str(), sl);
				_length += sl;
			}
			
			// Append Query Type and Class as last parameters, each one byte
			memcpy(&cdata[_length++], (char *)&type, 1);
			memcpy(&cdata[_length++], (char *)&_class, 1);
			cdata[sizeof(cdata)] = '\0';
			
			amods::connections::Connection *connection = module_factory->getConnection("udp");
			amods::connections::Request req = { "127.0.0.1", 53, 1 };
			amods::connections::Response response;
			
			std::cout << "DNS Start..." << std::endl;
			connection->setData(&cdata[0], sizeof(cdata));
			connection->SendRequest(req);
			response = connection->GetResponse();
			if (response.errnum > 0) {
				std::cout << response.error << std::endl;
			}
			std::cout << response.data << std::endl;
			std::cout << "DNS END..." << std::endl;
			
			delete connection;
		}

		/**
		* Parse the Response from the DNS Server
		* @param char *received Received Data
		* @param struct DnsResponse *res Response is saved in here
		*/
		void Dns::ParseResponse(char *received, DnsResponse *res) {
			
		}

		/**
		 * Get the corresponding numeric value based on the textual type
		 * @param std::string type
		 * @return short
		 */
		short Dns::getType(std::string type) const {
			std::transform(type.begin(), type.end(), type.begin(), ::toupper);
			std::map<short, std::string>::const_iterator it;
			for (it = _types.begin(); it != _types.end(); it++) {
				if (it->second == type) {
					return it->first;
				}
			}
		}

		/**
		 * Get the corresponding string value based on the numeric type
		 * @param short type
		 * @return std::string
		 */
		std::string Dns::getType(short type) const {
			std::map<short, std::string>::const_iterator it;
			it = _types.find(type);
			if (it == _types.end()) {
				return "";
			}
			return it->second;
		}

		/**
		 * Get the corresponding numeric value based on the textual class
		 * @param std::string _class
		 * @return short
		 */
		short Dns::getClass(std::string _class) const {
			std::transform(_class.begin(), _class.end(), _class.begin(), ::toupper);
			std::map<short, std::string>::const_iterator it;
			for (it = _classes.begin(); it != _classes.end(); it++) {
				if (it->second == _class) {
					return it->first;
				}
			}
		}

		/**
		 * Get the corresponding string value based on the numeric class
		 * @param short _class
		 * @return std::string
		 */
		std::string Dns::getClass(short _class) const {
			std::map<short, std::string>::const_iterator it;
			it = _classes.find(_class);
			if (it == _classes.end()) {
				return "";
			}
			return it->second;
		}

	}
}

// Register the DNS Monitor in the factory
extern "C" {
	int getEngineVersion() {
		return 1;
	}

	void registerPlugin(amods::Factory &factory) {
		using namespace amods::monitor;
		factory.GetMonitorManager().addModule(
			std::auto_ptr<Monitor>( new Dns(&factory) )
		);
	}
}
