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
			struct timeval t_begin, t_end;
			std::string _key, domain;
			uint16_t type = 1, _class = 1;
			uint16_t header_size = sizeof(struct DnsHeader);
			char dns_package[header_size];
			struct DnsHeader *header = (struct DnsHeader *)&dns_package;
			memset(header, 0, header_size);
			
			// Add a random UID and mark it as recursive.
			lukyluke::helper::RandomSeedInit();
			header->uid = (uint16_t)( 65535 * (rand() / (RAND_MAX + 1.0)) );
			header->type = 0;
			header->recursive = 1;
			header->num_questions = htons(1);
			
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
			type = htons(type);
			_class = htons(_class);
			
			// The domain is split out by the '.' and these are replaced by the length of the following domain part
			std::vector<std::string> data;
			std::size_t _pos = domain.find('.');
			uint16_t _length = 1; // Gives the total length of the domain (incl. one octet for NULL termination of the domain part)
			while (!domain.empty()) {
				if (_pos == std::string::npos) {
					_pos = domain.length();
				}
				data.push_back(domain.substr(0, _pos));
				_length += data.back().length() + 1; // Each domain part is prefixed with one byte which gives the number of chars of this part
				domain.erase(0, _pos + 1);
				_pos = domain.find('.');
			}
			
			// Calculate the size of the final char array we want to submit and copy the DNS-Package header
			char buffer[header_size + _length + 4]; // +4 for the type and class (each a uint16)
			memcpy(&buffer[0], &dns_package, header_size);
			
			// Invert the bits of the flags in the dns_header
			uint16_t *flags = (uint16_t *)&buffer[2];
			*flags = amods::connections::Connection::ReverseBits(*flags);
			*flags = htons(*flags);
			
			// Attach each domain part, prefixed with the number of chars
			_length = header_size;
			for (std::vector<std::string>::iterator it = data.begin(); it != data.end(); it++) {
				unsigned short sl = (*it).size();
				memcpy(&buffer[_length++], (char *)&sl, 1);
				memcpy(&buffer[_length], (*it).c_str(), sl);
				_length += sl;
			}
			
			// Append NULL termination of the domain
			buffer[_length++] = '\0';
			
			// Append Query Type and Class as last parameters, each one byte
			memcpy(&buffer[_length], (char *)&type, sizeof(uint16_t));
			_length += sizeof(uint16_t);
			memcpy(&buffer[_length], (char *)&_class, sizeof(uint16_t));
			_length += sizeof(uint16_t);
			
			// Submit the request
			System sys = GetSystem();
			amods::connections::Response response;
			amods::connections::Request req = { sys.address, 53, 1 };
			amods::connections::Connection *connection = module_factory->getConnection("udp");
			
			gettimeofday(&t_begin, NULL);
			connection->setData(&buffer[0], sizeof(buffer));
			connection->SendRequest(req);
			response = connection->GetResponse();
			gettimeofday(&t_end, NULL);
			if (response.errnum > 0) {
				std::cout << response.error << std::endl;
			}
			
			// Parse the response if there is one
			if (response.data_length > 0) {
				DnsResponse r;
				r.roundtrip = ((t_end.tv_sec * 1000.0) + (t_end.tv_usec / 1000.0)) - ((t_begin.tv_sec * 1000.0) + (t_begin.tv_usec / 1000.0));
				ParseResponse(response.data, response.data_length, &r);
			}
			
			delete [] response.data;
			delete connection;
		}

		/**
		* Parse the Response from the DNS Server
		* @param char *received Received Data
		* @param uint16_t length Number of chars/bytes received
		* @param struct DnsResponse *res Response is saved in here
		*/
		void Dns::ParseResponse(char *received, uint16_t length, DnsResponse *res) {
			// Split out the DNS header
			struct DnsHeader *header;
			char *data = (char *)(received + sizeof(DnsHeader));
			
			// The two bytes with the flags need to be inverted to store them 1:1 in the struct
			uint16_t *flags = (uint16_t *)(received + 2); // First two bytes is the UID
			*flags = ntohs(*flags);
			*flags = amods::connections::Connection::ReverseBits(*flags);
			
			header = (struct DnsHeader *)received;
			header->num_questions = ntohs(header->num_questions);
			header->num_answers = ntohs(header->num_answers);
			header->num_records = ntohs(header->num_records);
			header->num_additional = ntohs(header->num_additional);
			
			// Parse the answer if no error occured
			if (header->error == 0) {
				uint16_t i;
				uint8_t l;
				std::string part;
				
				// First get the questioned domains: One octet for the length followed by that num of chars
				// After the final NULL byte, the type and class, each two bytes, follow
				for (i = 0; i < header->num_questions; i++) {
					l = *data;
					std::pair<std::string, std::string> question = std::make_pair<std::string, std::string>("query", "");
					while (l > 0) {
						data++;
						question.second.append(std::string(data, l)).append(".");
						data += l;
						l = *data;
					}
					res->data.push_back(question);
					
					// Type and class, each 16bits - not interresting for now
					data++;
					data += 4;
				}
				
				// Check for answers
				for (i = 0; i < header->num_answers; i++) {
					
				}
			}
		}

		/**
		 * Get the corresponding numeric value based on the textual type
		 * @param std::string type
		 * @return uint16_t
		 */
		uint16_t Dns::getType(std::string type) const {
			std::transform(type.begin(), type.end(), type.begin(), ::toupper);
			std::map<uint16_t, std::string>::const_iterator it;
			for (it = _types.begin(); it != _types.end(); it++) {
				if (it->second == type) {
					return it->first;
				}
			}
		}

		/**
		 * Get the corresponding string value based on the numeric type
		 * @param uint16_t type
		 * @return std::string
		 */
		std::string Dns::getType(uint16_t type) const {
			std::map<uint16_t, std::string>::const_iterator it;
			it = _types.find(type);
			if (it == _types.end()) {
				return "";
			}
			return it->second;
		}

		/**
		 * Get the corresponding numeric value based on the textual class
		 * @param std::string _class
		 * @return uint16_t
		 */
		uint16_t Dns::getClass(std::string _class) const {
			std::transform(_class.begin(), _class.end(), _class.begin(), ::toupper);
			std::map<uint16_t, std::string>::const_iterator it;
			for (it = _classes.begin(); it != _classes.end(); it++) {
				if (it->second == _class) {
					return it->first;
				}
			}
		}

		/**
		 * Get the corresponding string value based on the numeric class
		 * @param uint16_t _class
		 * @return std::string
		 */
		std::string Dns::getClass(uint16_t _class) const {
			std::map<uint16_t, std::string>::const_iterator it;
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
