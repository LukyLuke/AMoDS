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

#ifndef DNS_MONITOR_H
#define DNS_MONITOR_H

#include "../monitor_manager.h"

namespace amods {
	namespace monitor {

		class Dns : public Monitor
		{
			public:
				Dns(Factory *factory);
				virtual ~Dns();
				Monitor* GetInstance(Factory *factory);
				Response BeginMonitor();

			private:
				
			protected:
				struct DnsResult {
					unsigned int h_len:4;          // length of the header
					unsigned int version:4;        // Version of IP
					unsigned char tos;             // Type of service
				};
				void ParseResponse(char *received, DnsResult *res);
				void SendRequest(Response * resp);
		};
	}
}
#endif // DNS_MONITOR_H
