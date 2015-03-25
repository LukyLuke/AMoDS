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
#include <string.h>

#include "echo.h"
#include "../../../factory.h"

namespace amods {
	namespace monitor {
		Echo::Echo(Factory *factory) {
			module_factory = factory;
			moduleName = "echo";
			moduleDescription = "Sends an ECHO-Ping to a remote Host and analyzes the response";
		}
		
		Monitor* Echo::GetInstance(Factory *factory) {
			return new Echo(factory);
		}

		Echo::~Echo() {
			if (sockraw > 0) {
				close(sockraw);
			}
			
			if (res.error.length() > 0) {
				std::cerr << "ICPM-Error: " << res.error << std::endl;
			}
			std::cout << "min: " << res.tmin << " / max: " << res.tmax << " / nrecv: " << res.nreceived << " / ntrans: " << res.ntransmitted << std::endl;
		}

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
			SendEchoRequest(system.num, system.timeout_ms);
			
			return resp;
		}

		/**
		* Send 'num' EchoPings
		* @param unsigned int num Number of EchoRequests to send
		*/
		void Echo::SendEchoRequest(unsigned int num, unsigned int timeout_ms) {
			seq_num = 1; // Initial sequence number for the icmp package
			
			// Standard-Response
			res.tsum = 0.0;
			res.tmin = 99999999.9;
			res.tmax = 0.0;
			res.nreceived = 0;
			res.ntransmitted = 0;
			res.error = "";
			
			// Check for a valid Destination
			unsigned int address;
			hostent *host = gethostbyname(system.address.c_str());
			if (!host) {
				address = inet_addr(system.address.c_str());
			}
			if (!host && (address == INADDR_NONE)) {
				res.error = "Host is not valid";
				return;
			}
			
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
			sockraw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
			if (sockraw < 0) {
				switch (errno) {
					case EPERM:           res.error = "Unable to create socket (no root)"; break;
					case EACCES:          res.error = "Unable to create socket (no permission)"; break;
					case EAFNOSUPPORT:    res.error = "Unable to create socket (family not supported)"; break;
					case EINVAL:          res.error = "Unable to create socket (unknown protocol/type)"; break;
					case EMFILE:          res.error = "Unable to create socket (too many open files or processes)"; break;
					case ENOBUFS:         res.error = "Unable to create socket (insufficient buffers available)"; break;
					case ENOMEM:          res.error = "Unable to create socket (insufficient memory available)"; break;
					case EPROTONOSUPPORT: res.error = "Unable to create socket (protocol or type not supported)"; break;
					default:              res.error = "Unable to create socket (unknown)"; std::cout << errno << std::endl; break;
				}
				return;
			}
			if (setsockopt(sockraw, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
				switch (errno) {
					case EBADF:       res.error = "Unable to set ReceiveTimeout (not a valid socket)"; break;
					case EFAULT:      res.error = "Unable to set ReceiveTimeout (invalid memory for value)"; break;
					case EINVAL:      res.error = "Unable to set ReceiveTimeout (invalid length)"; break;
					case ENOPROTOOPT: res.error = "Unable to set ReceiveTimeout (unknown option on SOL_SOCKET level)"; break;
					case ENOTSOCK:    res.error = "Unable to set ReceiveTimeout (socked var defines a file, not a socket)"; break;
					default:          res.error = "Unable to set ReceiveTimeout (unknown)"; break;
				}
				return;
			}
			if (setsockopt(sockraw, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
				switch (errno) {
					case EBADF:       res.error = "Unable to set SendTimeout (not a valid socket)"; break;
					case EFAULT:      res.error = "Unable to set SendTimeout (invalid memory for value)"; break;
					case EINVAL:      res.error = "Unable to set SendTimeout (invalid length)"; break;
					case ENOPROTOOPT: res.error = "Unable to set SendTimeout (unknown option on SOL_SOCKET level)"; break;
					case ENOTSOCK:    res.error = "Unable to set SendTimeout (socked var defines a file, not a socket)"; break;
					default:          res.error = "Unable to set SendTimeout (unknown)"; break;
				}
				return;
			}
			
			// Send 'num' requests
			int bytes_wrote, bytes_read;
			char recv_buffer[MAX_PACKET_SIZE];
			for (int i = 0; i < num; i++) {
				if (i > 0) {
					MicroSleep(timeout_ms);
				}
				
				// Read data after the request was sent.
				SendRequest();
				
				// The function recvfrom throws an error EFAULT (Bad Address) and returnd -1 but receives data.
				// Therefore we reset the receiving buffer and check if we got some data.
				recv_buffer[0] = '\0';
				if ((bytes_read = recvfrom(sockraw, recv_buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&received_from, (socklen_t *)sizeof(received_from))) < 0) {
					if ((errno == EAGAIN) || (recv_buffer[0] != '\0')) {
						bytes_read = PACKET_SIZE;
					}
					else {
						std::cout << "Read bytes: " << bytes_read << ", should be " << PACKET_SIZE << std::endl;
						std::cout << "Error (" << errno << "):" << strerror(errno) << std::endl;
						continue;
					}
				}
				
				// Decode Data and update the result
				ParseResponse(recv_buffer, bytes_read, &received_from);
			}
		}

		/**
		 * Send out an ICMP ECHO Request
		 * 
		 * The first 8 bytes are reserved for a timeval struct to calculate the
		 * round-trip of the package.
		 * @return int Number of bytes written
		 */
		int Echo::SendRequest() {
			int bytes_wrote, i, packet_size = DEF_PACKET_SIZE + 8;
			
			// icmp_packet is the data which is sent. The first part is for the ICMP-Header
			// followed by 8 bytes for a timeval structure for the roundtrip and after some dummy data.
			static char icmp_packet[MAX_PACKET_SIZE];
			struct icmphdr *icmp_header = (struct icmphdr *)&icmp_packet; // icmp_header as first
			struct timeval *tp = (struct timeval *)&icmp_packet[8]; // 8 bytes for the timeval
			char *datap = &icmp_packet[8 + sizeof(struct timeval)]; // rest for dummy data
			
			// Add the timeval with current time and fill the rest (after 8 bytes) with some int values
			gettimeofday(tp, NULL);
			for (i = 8; i < DEF_PACKET_SIZE; i++) {
				*datap++ = i;
			}
			
			// Fill the icmp header data - Checksum has to be '0' to calculate the a valid checksum :)
			icmp_header->i_type = (EchoCodes)echo_request;
			icmp_header->i_code = 0;
			icmp_header->i_id = _icmp_header_id;
			icmp_header->i_seq = seq_num++;
			icmp_header->i_cksum = 0;
			icmp_header->i_cksum = Checksum((unsigned short *)icmp_packet, packet_size);

			// Send the packet and check for an error
			bytes_wrote = sendto(sockraw, icmp_packet, packet_size, 0, (struct sockaddr *)&destination, sizeof(struct sockaddr));
			if (bytes_wrote < 0) {
				res.error = "Error sending data to host";
				res.errnum = errno;
				res.errmsg = strerror(errno);
				return bytes_wrote;
			}
			res.ntransmitted++;
			return bytes_wrote;
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
		* @param char *received Received Data
		* @param int bytes_read Number of received bytes
		* @param struct sockaddr_in *from Where is this response from_length
		*/
		void Echo::ParseResponse(char *received, int bytes_read, struct sockaddr_in *from) {
			struct iphdr *ip_header;
			struct icmphdr *icmp_header;
			unsigned short ip_header_length;
			double trip_time;
			struct timeval *roundtrip, tstamp;
			gettimeofday(&tstamp, NULL);
			
			ip_header = (struct iphdr *)received;
			ip_header_length = ip_header->h_len << 2; // Length in Bytes = 32-bit words * 4 (shift two bytes away)
			
			// ICMP-Header is 8 Bytes, so we must have received more than the IP-Header + Icmp-Header length
			if (bytes_read < ICMP_HEADER_LENGTH + ip_header_length) {
				res.errnum = EMSGSIZE;
				res.errmsg = "Too few bytes received";
				return;
			}
			bytes_read -= ip_header_length;
			
			// Checkfor a valid Response, icmp header starts after the ip header
			icmp_header = (struct icmphdr *)(received + ip_header_length);
			if (icmp_header->i_type != (EchoCodes)echo_reply) {
				std::cout << "Err type: " << (unsigned int)icmp_header->i_type << "!=" << (EchoCodes)echo_reply << std::endl;
				res.errnum = EPROTOTYPE;
				res.errmsg = "Wrong ICMP type received: ";
				res.errmsg.append((char *)(icmp_header->i_type));
				return;
			}
			
			// This is not our package
			if (icmp_header->i_id != _icmp_header_id) {
				res.errnum = EPROTO;
				res.errmsg = "Someone elses package received.";
				return;
			}
			
			// Read out the timeval for calculate the roundtrip
			roundtrip = (struct timeval *)(received + ip_header_length + ICMP_HEADER_LENGTH);
			trip_time = ((tstamp.tv_sec * 1000.0) + (tstamp.tv_usec / 1000.0)) - ((roundtrip->tv_sec * 1000.0) + (roundtrip->tv_usec / 1000.0));
			
			// Set PingStatistics values
			res.tsum += trip_time;
			res.tmin = (trip_time < res.tmin) ? trip_time : res.tmin;
			res.tmax = (trip_time > res.tmax) ? trip_time : res.tmax;
			res.nreceived++;
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
			std::auto_ptr<Monitor>( new Echo(&factory) )
		);
	}
}
