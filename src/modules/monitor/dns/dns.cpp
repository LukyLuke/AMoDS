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

#include <string>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/time.h>

#include "dns.h"
#include "../../../factory.h"

namespace amods {
	namespace monitor {
		Dns::Dns(Factory *factory) {
			moduleName = "dns";
			moduleDescription = "Check a DNS Server for a defined record or for a functionality";
			module_factory = factory;
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
			resp.header[GetSystem().num];
			
			SendRequest(&resp);
			return resp;
		}

		/**
		* Check the DNS Server.
		* @param Response *resp The reponse object with statistical data
		*/
		void Dns::SendRequest(Response *resp) {
			// Build the request package, all flags to '0' but recursive to '1'
			DnsFlags header = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 };
			
			amods::connections::Connection *connection = module_factory->getConnection("udp");
			amods::connections::Request req = { "127.0.0.1", 53, 1, "test dns lookup" };
			amods::connections::Response response;
			
			std::cout << "DNS Start..." << std::endl;
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
