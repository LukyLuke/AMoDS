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
    
    Hypercube cube;
    cube.AddNode(new HyperNode("Label eins", "192.168.22.11"));
    cube.AddNode(new HyperNode("Label zwei", "192.168.22.12"));
    cube.AddNode(new HyperNode("Label drei", "192.168.22.13"));
    cube.AddNode(new HyperNode("Label vier", "192.168.22.14"));
    cube.AddNode(new HyperNode("Label fünf", "192.168.22.15"));
    cube.BuildHypercube();
    cube.debug();
    
    int i;
    std::vector<HyperNode*>* nodes;
    nodes = cube.GetNodes();
    for (i = 0; i < nodes->size(); i++) {
      nodes->at(i)->debug();
    }
    
    cube.Reset();
    
    for (i = 0; i < nodes->size(); i++) {
      nodes->at(i)->debug();
    }
    
    std::cout << "thread: " << "\n";
    std::cout << lukyluke::helper::binomial(96, 20) << "\n";
    std::cout.flush();
    //sleep(2);
    std::cout << "done\n";
  }

}
