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

#include <bitset>

#include "udp.h"
#include "../../../factory.h"

namespace amods {
	namespace connections {
		UDP::UDP(Factory *factory) {
			module_factory = factory;
			moduleName = "udp";
			moduleDescription = "UDP Connection Handler";
		}
		
		UDP::~UDP() {
			if (sockraw > 0) {
				close(sockraw);
			}
		}
		
		Connection* UDP::GetInstance(Factory *factory) {
			return new UDP(factory);
		}
		
		void UDP::SendRequest(Request req) {
			request = req;
			
			// Check for a valid Destination
			unsigned int address;
			hostent *host = gethostbyname(request.host.c_str());
			if (!host) {
				address = inet_addr(request.host.c_str());
			}
			if (!host && (address == INADDR_NONE)) {
				response.error = "Host is not valid";
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
			destination.sin_port = htons(request.port);
			std::cout << "UDP-Connection: " << inet_ntoa(destination.sin_addr) << std::endl;
			
			// may 5 seconds for sending and receiving
			struct timeval timeout;
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;
			
			// Create the Socket and set timeout-options
			sockraw = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (sockraw < 0) {
				response.errnum = errno;
				response.errmsg = strerror(errno);
				switch (errno) {
					case EPERM:           response.error = "Unable to create socket (no root)"; break;
					case EACCES:          response.error = "Unable to create socket (no permission)"; break;
					case EAFNOSUPPORT:    response.error = "Unable to create socket (family not supported)"; break;
					case EINVAL:          response.error = "Unable to create socket (unknown protocol/type)"; break;
					case EMFILE:          response.error = "Unable to create socket (too many open files or processes)"; break;
					case ENOBUFS:         response.error = "Unable to create socket (insufficient buffers available)"; break;
					case ENOMEM:          response.error = "Unable to create socket (insufficient memory available)"; break;
					case EPROTONOSUPPORT: response.error = "Unable to create socket (protocol or type not supported)"; break;
					default:              response.error = "Unable to create socket (unknown)"; std::cout << errno << std::endl; break;
				}
				return;
			}
			if (setsockopt(sockraw, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
				response.errnum = errno;
				response.errmsg = strerror(errno);
				switch (errno) {
					case EBADF:       response.error = "Unable to set SendTimeout (not a valid socket)"; break;
					case EFAULT:      response.error = "Unable to set SendTimeout (invalid memory for value)"; break;
					case EINVAL:      response.error = "Unable to set SendTimeout (invalid length)"; break;
					case ENOPROTOOPT: response.error = "Unable to set SendTimeout (unknown option on SOL_SOCKET level)"; break;
					case ENOTSOCK:    response.error = "Unable to set SendTimeout (socked var defines a file, not a socket)"; break;
					default:          response.error = "Unable to set SendTimeout (unknown)"; break;
				}
				return;
			}
			if ((request.response_data > 0) && (setsockopt(sockraw, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0)) {
				response.errnum = errno;
				response.errmsg = strerror(errno);
				switch (errno) {
					case EBADF:       response.error = "Unable to set ReceiveTimeout (not a valid socket)"; break;
					case EFAULT:      response.error = "Unable to set ReceiveTimeout (invalid memory for value)"; break;
					case EINVAL:      response.error = "Unable to set ReceiveTimeout (invalid length)"; break;
					case ENOPROTOOPT: response.error = "Unable to set ReceiveTimeout (unknown option on SOL_SOCKET level)"; break;
					case ENOTSOCK:    response.error = "Unable to set ReceiveTimeout (socked var defines a file, not a socket)"; break;
					default:          response.error = "Unable to set ReceiveTimeout (unknown)"; break;
				}
				return;
			}
			
			// Send out the data and possibly wait dor returned data.
			int bytes_wrote, bytes_read;
			char packet[MAX_DATA_SIZE];
			char buffer[MAX_DATA_SIZE];
			memcpy(&packet[0], data, MAX_DATA_SIZE - 1);
			packet[MAX_DATA_SIZE] = '\0';
			
			bytes_wrote = sendto(sockraw, packet, sizeof(packet), 0, (struct sockaddr *)&destination, sizeof(struct sockaddr));
			if (request.response_data > 0) {
				bytes_read = recvfrom(sockraw, buffer, sizeof(buffer), 0, (struct sockaddr *)&received_from, (socklen_t *)sizeof(received_from));
				response.data.clear();
				response.data.append(buffer);
			}
			
			std::cout << "Request: " << data << std::endl;
		}
		
		Response UDP::GetResponse() {
			return response;
		}
		
	}
}


// Register the UDP-Connection EntryFunctions
extern "C" {
	int getEngineVersion() {
		return 1;
	}
	
	void registerPlugin(amods::Factory &factory) {
		using namespace amods::connections;
		factory.GetConnectionsManager().addModule(
			std::auto_ptr<Connection>( new UDP(&factory) )
		);
	}
}
