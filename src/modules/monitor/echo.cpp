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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>

#include "echo.h"
#include "monitor_manager.h"
#include "../../factory.h"
#include <string.h>

namespace amods {
	namespace monitor {
		
		Echo::Echo() {
			moduleName = "echo";
			moduleDescription = "Sends an ECHO-Ping to a remote Host and analyzes the response";
		}
		
		Echo::~Echo() {}
		
		Response Echo::BeginMonitor() {
			Response resp;
			resp.num = GetSystem().num;
			resp.min = 0.0;
			resp.max = 0.0;
			resp.avg = 0.0;
			resp.times[GetSystem().num];
			resp.data[GetSystem().num];
			resp.header[GetSystem().num];
			
			// ICMP Header-ID to check for a valid response
			_icmp_header_id = (unsigned short) syscall(SYS_gettid);
			
			//system.address = "192.168.22.250";
			
			if (system.address.empty()) {
				return resp;
			}
			if (!system.num) {
				system.num = 1;
			}
			if (!system.timeout_ms) {
				system.timeout_ms = 1000;
			}
			
			// Create the statistics and start the Monitoring
			pingstat res;
			SendEchoRequest(&res, system.num, system.timeout_ms);
			
			if (res.error.length() > 0) {
				std::cerr << "ICPM-Error: " << res.error << std::endl;
			}
			std::cout << "min: " << res.tmin << " / max: " << res.tmax << " / nrecv: " << res.nreceived << " / ntrans: " << res.ntransmitted << std::endl;
			
			return resp;
		}
		
		/**
		* Send 'num' EchoPings
		* @param pingstat *res PingStatistics to fill
		* @param unsigned int num Number of EchoRequests to send
		*/
		void Echo::SendEchoRequest(pingstat *res, unsigned int num, unsigned int timeout_ms) {
			// Standard-Response
			res->tsum = 0.0;
			res->tmin = 99999999.9;
			res->tmax = 0.0;
			res->nreceived = 0;
			res->ntransmitted = 0;
			res->error = "";
			
			// Check for a valid Destination
			unsigned int address;
			hostent *host = gethostbyname(system.address.c_str());
			if (!host) {
				address = inet_addr(system.address.c_str());
			}
			if (!host && (address == INADDR_NONE)) {
				res->error = "Host is not valid";
				return;
			}
			
			// Fill the Destination with right data
			struct sockaddr_in destination, received_from;
			socklen_t from_length = sizeof(received_from);
			
			memset(&received_from, 0, sizeof(received_from));
			if (host != NULL) {
				memcpy(&(destination.sin_addr), host->h_addr, host->h_length);
				destination.sin_family = (unsigned char) host->h_addrtype;
			} else {
				destination.sin_addr.s_addr = address;
				destination.sin_family = AF_INET;
			}
			destination.sin_port = htons(53);
			std::cout << "(105) ping: " << inet_ntoa(destination.sin_addr) << std::endl;
			
			// may 5 seconds for sending and receiving
			struct timeval timeout;
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;
			
			// Create the Socket and set timeout-options
			int sockraw;
			sockraw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
			if (sockraw < 0) {
				switch (errno) {
					case EPERM:           res->error = "Unable to create socket (no root)"; break;
					case EACCES:          res->error = "Unable to create socket (no permission)"; break;
					case EAFNOSUPPORT:    res->error = "Unable to create socket (family not supported)"; break;
					case EINVAL:          res->error = "Unable to create socket (unknown protocol/type)"; break;
					case EMFILE:          res->error = "Unable to create socket (too many open files or processes)"; break;
					case ENOBUFS:         res->error = "Unable to create socket (insufficient buffers available)"; break;
					case ENOMEM:          res->error = "Unable to create socket (insufficient memory available)"; break;
					case EPROTONOSUPPORT: res->error = "Unable to create socket (protocol or type not supported)"; break;
					default:              res->error = "Unable to create socket (unknown)"; std::cout << errno << std::endl; break;
				}
				return;
			}
			if (setsockopt(sockraw, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
				switch (errno) {
					case EBADF:       res->error = "Unable to set ReceiveTimeout (not a valid socket)"; break;
					case EFAULT:      res->error = "Unable to set ReceiveTimeout (invalid memory for value)"; break;
					case EINVAL:      res->error = "Unable to set ReceiveTimeout (invalid length)"; break;
					case ENOPROTOOPT: res->error = "Unable to set ReceiveTimeout (unknown option on SOL_SOCKET level)"; break;
					case ENOTSOCK:    res->error = "Unable to set ReceiveTimeout (socked var defines a file, not a socket)"; break;
					default:          res->error = "Unable to set ReceiveTimeout (unknown)"; break;
				}
				return;
			}
			if (setsockopt(sockraw, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
				switch (errno) {
					case EBADF:       res->error = "Unable to set ReceiveTimeout (not a valid socket)"; break;
					case EFAULT:      res->error = "Unable to set ReceiveTimeout (invalid memory for value)"; break;
					case EINVAL:      res->error = "Unable to set ReceiveTimeout (invalid length)"; break;
					case ENOPROTOOPT: res->error = "Unable to set ReceiveTimeout (unknown option on SOL_SOCKET level)"; break;
					case ENOTSOCK:    res->error = "Unable to set ReceiveTimeout (socked var defines a file, not a socket)"; break;
					default:          res->error = "Unable to set ReceiveTimeout (unknown)"; break;
				}
				return;
			}
			
			// Create the ICMP-Data and fill it with '0'
			char recv_buffer[PACKET_SIZE];
			char icmp_data[PACKET_SIZE];
			memset(icmp_data, 0, PACKET_SIZE);
			
			// map the first part of the ICMP-Data as the ICMP-Header
			// that way we can use the struct to define all values and fill the memory
			struct icmphdr *icmp_header = (struct icmphdr *) &icmp_data;
			icmp_header->i_type = (EchoCodes)echo_request;
			icmp_header->i_code = 0;
			icmp_header->i_cksum = 0;
			icmp_header->i_seq = 0;
			icmp_header->i_id = _icmp_header_id;
			
			// Fill the whole Datapart of the ICMP-Data with 'E' as Pseudo-Data
			char *datapart;
			datapart = icmp_data + sizeof(struct icmphdr);
			memset(datapart, 'E', DEF_PACKET_SIZE);
			
			// Send 'num' requests
			unsigned short seq_num = 0;
			int bytes_wrote, bytes_read;
			for (int i = 0; i < num; i++) {
				struct timeval tstamp;
				gettimeofday(&tstamp, NULL);
				
				// Prepare the Header for this package
				//struct icmphdr *icmp_header = (struct icmphdr *) &icmp_data;
				//icmp_header->i_type = (EchoCodes)echo_request;
				icmp_header->i_seq = seq_num++;
				icmp_header->timestamp = ((tstamp.tv_sec * 1000.0) + (tstamp.tv_usec / 1000.0));
				icmp_header->i_cksum = Checksum((unsigned short *)icmp_data, DEF_PACKET_SIZE + sizeof(struct icmphdr));
				
				// Send data
				bytes_wrote = sendto(sockraw, icmp_data, DEF_PACKET_SIZE + sizeof(struct icmphdr), 0, (struct sockaddr *)&destination, sizeof(destination));
				if (bytes_wrote < 0) {
					if (errno == ETIMEDOUT) {
						continue;
					}
					res->error = "Error sending data to host";
					return;
				}
				res->ntransmitted++;
				
				// Read data
				bytes_read = recvfrom(sockraw, recv_buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&received_from, &from_length);
				if (bytes_read < 0) {
					if (errno == ETIMEDOUT) {
						continue;
					}
					//res.error = "Error receiving data from host";
					//return res;
				}
				
				// Decode Data and update the result
				ParseResponse(res, recv_buffer, bytes_read, &received_from);
				
				// Wait a Second
				if (i+1 < num) {
					MicroSleep(timeout_ms);
				}
			}
			close(sockraw);
		}
		
		/**
		* Create the Checksum for the ICMP-Header
		* @param unsigned short *buffer Buffer to create the Checksum for
		* @param int size Size of the Buffer
		* @return unsigned short
		*/
		unsigned short Echo::Checksum(unsigned short *buffer, int size) {
			unsigned long checksum = 0;
			
			while (size > 1) {
				checksum += *buffer++;
				size -= sizeof(unsigned short);
			}
			
			if (size) {
				size -= sizeof(unsigned short);
			}
			
			checksum  = (checksum >> 16) + (checksum & 0xffff);
			checksum += (checksum >> 16);
			return (unsigned short) (~checksum);
		}

		/**
		* Parse the Response of an ICMP EchoPing and fill the pingstatistics
		* @param pingstat *resp Ping-Statistics to update
		* @param char *received Received Data
		* @param int bytes_read Number of received bytes
		* @param struct sockaddr_in *from Where is this response from_length
		*/
		void Echo::ParseResponse(struct pingstat *resp, char *received, int bytes_read, struct sockaddr_in *from) {
			struct iphdr *ip_header;
			struct icmphdr *icmp_header;
			struct timeval timestamp;
			unsigned short ip_header_length;
			double trip_time, calc_time;
			
			ip_header = (struct iphdr *) received;
			ip_header_length = ip_header->h_len * 4; // Length in Bytes = 32-bit words * 4

			// ICMP-Header is 8 Bytes, so we must have received more than the IP-Header + Icmp-Header length
			if (bytes_read < ICMP_HEADER_LENGTH + ip_header_length) {
				std::cout << "Too few bytes received" << std::endl;
				return;
			}

			// Checkfor a valid Response
			icmp_header = (struct icmphdr *) (received + ip_header_length);
			if (icmp_header->i_type != (EchoCodes)echo_reply) {
				std::cout << "Err type: " << (unsigned int)icmp_header->i_type << "!=" << (EchoCodes)echo_reply << std::endl;
				return;
			}

			// this is not our package
			if (icmp_header->i_id != _icmp_header_id) {
				std::cout << "Captured someone elses packet, sorry :)" << std::endl;
				return;
			}

			// Calculate the roundtrip
			gettimeofday(&timestamp, NULL);
			calc_time = (timestamp.tv_sec*1000.0 + timestamp.tv_usec/1000.0);
			trip_time = calc_time - icmp_header->timestamp;

			std::cout << "Valid packed with round trip: " << (unsigned int) trip_time << std::endl;

			// Set PingStatistics values
			resp->tsum += trip_time;
			resp->tmin = (trip_time < resp->tmin) ? trip_time : resp->tmin;
			resp->tmax = (trip_time > resp->tmax) ? trip_time : resp->tmax;
			resp->nreceived++;
		}

		/**
		* Sleep for the given number of Microseconds
		* @param unsigned int ms Number Microseconds to sleep
		*/
		void Echo::MicroSleep(unsigned int ms) {
			struct timeval tv;
			tv.tv_sec = ms / 1000;
			tv.tv_usec = ms % 1000;
			select(0, NULL, NULL, NULL, &tv);
		}
		
	}
}

// Register the UDP-Connection EntryFunctions
extern "C" {
	int getEngineVersion() {
		return 1;
	}
	
	void registerPlugin(amods::Factory &factory) {
		using namespace amods::monitor;
		factory.GetMonitorManager().addModule(
			std::auto_ptr<Monitor>( new Echo() )
		);
	}
}
