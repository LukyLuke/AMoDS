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
#include <unistd.h>
#include <pthread.h>

#include "amods.h"
#include "helper/math.h"
#include "hypercube.h"
#include "factory.h"

namespace amods {
  AMoDS::AMoDS() { }
  AMoDS::~AMoDS() { }

  /**
   * Called before the Execute - to setup anything
   * @virtual
   * @protected
   */
  void AMoDS::Setup() {
    Thread::Setup();
    // TODO: Do any Setup here
  }
  
  /**
   * Called by the Thread just after Setup - Execute all Code here
   * @virtual
   * @protected
   */
  void AMoDS::Execute(void *) {
    
    /*Hypercube cube, cube1, cube2, cube3;
    cube1.AddNode(new HyperNode("Label eins", "192.168.22.11"));
    cube1.AddNode(new HyperNode("Label zwei", "192.168.22.12"));
    cube1.AddNode(new HyperNode("Label drei", "192.168.22.13"));
    cube1.AddNode(new HyperNode("Label vier", "192.168.22.14"));
    cube1.AddNode(new HyperNode("Label fünf", "192.168.22.15"));
    cube1.BuildHypercube();
    
    cube2.AddNode(new HyperNode("Label zwei eins", "192.168.23.11"));
    cube2.AddNode(new HyperNode("Label zwei zwei", "192.168.23.12"));
    cube2.AddNode(new HyperNode("Label zwei drei", "192.168.23.13"));
    cube2.AddNode(new HyperNode("Label zwei vier", "192.168.23.14"));
    cube2.AddNode(new HyperNode("Label zwei fünf", "192.168.23.15"));
    cube2.BuildHypercube();
    
    cube3.AddNode(new HyperNode("Label drei drei", "192.168.24.13"));
    cube3.AddNode(new HyperNode("Label drei vier", "192.168.24.14"));
    cube3.AddNode(new HyperNode("Label drei fünf", "192.168.24.15"));
    cube3.BuildHypercube();
    
    cube.AddNode(new HyperNode("Network 1", &cube1));
    cube.AddNode(new HyperNode("Network 2", &cube2));
    cube.AddNode(new HyperNode("Network 3", &cube3));
    cube.BuildHypercube();
    
    cube.debug();*/
    
    /*int i;
    std::vector<HyperNode*>* nodes;
    nodes = cube.GetNodes();
    for (i = 0; i < nodes->size(); i++) {
      nodes->at(i)->debug();
    }*/
    
    const std::string m = std::string("src/modules/connection/libudp.so");
    Factory moduleFactory;
    moduleFactory.LoadPlugin(m);
    std::cout << "factory loaded" << std::endl;
    
    amods::connections::Connection *con = moduleFactory.getConnection("udp");
    amods::connections::Request req = { "Data" };
    con->SendRequest(req);
    delete con;
    
    // Why can I not destroy the factory here?
    //delete &moduleFactory;
    //std::cout << "factory destroyed" << std::endl;
    
    std::cout << "amods finished" << std::endl;
  }

}
