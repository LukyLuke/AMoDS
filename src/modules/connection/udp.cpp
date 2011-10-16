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

#include <iostream>
#include <string>
#include <memory>

#include "udp.h"
#include "connection_manager.h"
#include "../../factory.h"

namespace amods {
  namespace connections {
    UDP::UDP() {
      moduleName = "udp";
      moduleDescription = "UDP Connection Handler";
      std::cout << "Create UPD" << std::endl;
    }
    
    UDP::~UDP() {
      std::cout << "Destroy UPD" << std::endl;
    }
    
    Connection* UDP::GetInstance() {
      std::cout << "INININ" << std::endl;
      return new UDP();
    }
    
    void UDP::SendRequest(Request req) {
      request = req;
      std::cout << "Request: " << request.data << std::endl;
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
      std::auto_ptr<Connection>( new UDP() )
    );
  }
}
