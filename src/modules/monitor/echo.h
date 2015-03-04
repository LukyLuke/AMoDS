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

#ifndef ECHO_H
#define ECHO_H

#include <netinet/in.h>

#include "monitor_manager.h"

namespace amods {
	namespace monitor {

		class Echo : public Monitor
		{
		public:
			enum EchoCodes {
				echo_reply = 0,
				destination_unreachable = 3,
				source_quench = 4,
				redirect = 5,
				echo_request = 8,
				time_exceeded = 11,
				parameter_problem = 12,
				timestamp_request = 13,
				timestamp_reply = 14,
				info_request = 15,
				info_reply = 16,
				address_request = 17,
				address_reply = 18
			};

			struct pingstat {
				double tmin;
				double tmax;
				double tsum;
				int ntransmitted;
				int nreceived;
				std::string error;
			};

			Echo();
			virtual ~Echo();
			Monitor* GetInstance() { return new Echo(); };
			Response BeginMonitor();

		private:

		protected:
			struct iphdr {
				unsigned int h_len:4;          // length of the header
				unsigned int version:4;        // Version of IP
				unsigned char tos;             // Type of service
				unsigned short total_len;      // total length of the packet
				unsigned short ident;          // unique identifier
				unsigned short frag_and_flags; // flags
				unsigned char  ttl;
				unsigned char proto;           // protocol (TCP, UDP etc)
				unsigned short checksum;       // IP checksum
				unsigned int source_ip;
				unsigned int dest_ip;
			};

			struct icmphdr {
				unsigned char i_type;
				unsigned char i_code;   // type sub code
				unsigned short i_cksum;
				unsigned short i_id;
				unsigned short i_seq;
				double timestamp;       // This is not the std header, but we reserve space for time roundtrip calculation
			};

			struct icmproute {
				unsigned char code;
				unsigned char h_len:4;
				unsigned char ptr_offset;
				unsigned char ip_addr[9];
			};

			static const unsigned int ICMP_HEADER_LENGTH = 8;
			static const unsigned int DEF_PACKET_SIZE = 32;
			static const unsigned int MAX_PACKET_SIZE = 1024;
			static const unsigned int PACKET_SIZE = (sizeof(struct icmphdr) + MAX_PACKET_SIZE);

			unsigned char _icmp_header_id;

			void SendEchoRequest(pingstat *res) { return SendEchoRequest(res, 1, 1000); };
			void SendEchoRequest(pingstat *res, unsigned int num);
			void SendEchoRequest(pingstat *res, unsigned int num, unsigned int timeout_ms);
			unsigned short Checksum(unsigned short *buffer, int size);
			void ParseResponse(pingstat *resp, char *received, int bytes_read, struct sockaddr_in *from);
			void MicroSleep(unsigned int ms);
		};

	}
}
#endif // ECHO_H
