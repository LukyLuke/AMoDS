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

#ifndef DNS_MONITOR_H
#define DNS_MONITOR_H

#include <string>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <map>
#include <vector>

#include "../../../factory.h"
#include "../../../helper/random.h"
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
				// Not a complete list, only the ones I think can be usefull
				std::map<short, std::string> _types;
				void _create_types() {
					_types[1] = "A";
					_types[2] = "NS";
					_types[5] = "CNAME";
					_types[6] = "SOA";
					_types[11] = "WKS";
					_types[12] = "PTR";
					_types[13] = "HINFO";
					_types[14] = "MINFO";
					_types[15] = "MX";
					_types[16] = "TXT";
					_types[33] = "SRV";
					_types[28] = "AAAA";
					_types[37] = "CERT";
					_types[43] = "DS";
					_types[48] = "DNSKEY";
					_types[60] = "CDNSKEY";
					_types[251] = "IXFR";
					_types[252] = "AXFR";
					_types[253] = "MAILB";
					_types[254] = "MAILA";
					_types[255] = "*";
				};
				std::map<short, std::string> _classes;
				void _create_classes() {
					_classes[1] = "IN";
					_classes[2] = "CS";
					_classes[3] = "CH";
					_classes[4] = "HS";
					_classes[255] = "*";
				};
				
			protected:
				struct DnsHeader {
					unsigned short uid;
					unsigned short type:1; // QR -  0: query, 1: response
					unsigned short opcode:4; // OPCODE - 0: query, 1: iquery, 2: status, rest reserverd
					unsigned short authoritative:1; // AA - Set to 1 in responses if the server is the authoritative one
					unsigned short truncated:1; // TC - Set to 1 if the message was truncated due to too much data
					unsigned short recursive:1; // RD - Recursion desired
					unsigned short recursion:1; // RA - Recursion available
					unsigned short _reserved:3; // Z - Reserved
					unsigned short error:4; // RCODE - 0: No error, 1:Format error, 2: Server failure, 3: Name error, 4: Not implemented, 5: Refused, rest reserved
					unsigned short num_questions; // QCOUNT
					unsigned short num_answers; // ANCOUNT
					unsigned short num_records; // NSCOUNT
					unsigned short num_additional; // ARCOUNT
				};
				struct DnsResponse {
					DnsHeader header;
					unsigned char type:4;
					unsigned int ttl:8;
					std::vector<std::string> data;
				};
				void ParseResponse(char *received, DnsResponse *res);
				void SendRequest(Response *resp);
				short getType(std::string type) const;
				std::string getType(short type) const;
				short getClass(std::string type) const;
				std::string getClass(short type) const;
		};
	}
}
#endif // DNS_MONITOR_H
