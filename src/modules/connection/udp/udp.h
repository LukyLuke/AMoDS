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

#ifndef UDP_H
#define UDP_H

#include <string>
#include <netinet/in.h>

#include "../connection_manager.h"

namespace amods {
	namespace connections {
		class UDP : public Connection
		{
			private:
				
			protected:
				static const unsigned int MAX_DATA_SIZE = 65507; // 2^16 - IP Header - UDP-Header
				
				Request request;
				Response response;
				int sockraw;
				struct sockaddr_in destination, received_from;
				
			public:
				UDP(Factory *factory);
				virtual ~UDP();
				Connection* GetInstance(Factory *factory);
				void SendRequest(Request req);
				Response GetResponse();
			};
	}
}
#endif // UDP_H
