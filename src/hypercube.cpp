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

#include<iostream>
#include<cmath>
#include "hypercube.h"
#include "helper/math.h"

namespace amods
{
  Hypercube::Hypercube () {
    num_nodes = 0;
    num_connections = 0;
    dimension = 0;
  }
  
  Hypercube::Hypercube (uint32_t dimension) {
    Hypercube();
    Dimension(dimension);
  }

  Hypercube::~Hypercube () {
    // We destroy all Nodes in this Hypercube if we destroy the Hypercube
    ClearNodes();
  }

  void Hypercube::CalculateConnections () {
    num_connections = 0;
    if (dimension > 0) {
      num_connections = (uint32_t)lukyluke::helper::binomial(dimension, 1) * (uint32_t)std::pow(2, dimension - 1);
    } else {
      num_connections = 0;
    }
  }
  
  void Hypercube::CalculateNodes () {
    num_nodes = 0;
    if (dimension > 0) {
      num_nodes = (uint32_t)std::pow(2, dimension);
    } else {
      dimension = 0;
    }
  }
  
  void Hypercube::CalculateDimension() {
    dimension = 0;
    while ( std::pow(2, dimension) < num_nodes ) {
      dimension++;
    }
    num_nodes = std::pow(2, dimension);
  }
  
  std::vector<HyperNode*>* Hypercube::GetNodes() {
    return &nodes;
  }
  
  void Hypercube::ClearNodes() {
    Reset();
    while (!nodes.empty()) {
      delete nodes.back();
      nodes.pop_back();
    }
  }
  
  void Hypercube::BuildFromNodes(std::vector<HyperNode*>* new_nodes) {
     ClearNodes();
     AddNodes(new_nodes);
     BuildHypercube();
  }
  
  void Hypercube::AddNode(HyperNode* node) {
    nodes.push_back(node);
  }
  
  void Hypercube::AddNodes(std::vector<HyperNode*>* nodes) {
    std::vector<HyperNode*>::iterator node;
    for (node = nodes->begin(); node < nodes->end(); node++) {
      AddNode(*node);
    }
  }
  
  void Hypercube::Reset() {
    std::vector<HyperNode*>::iterator node;
    for (node = nodes.begin(); node < nodes.end(); node++) {
      (*node)->ClearConnections();
    }
  }
  
  void Hypercube::BuildHypercube() {
    uint32_t i, j, dim, step, real_nodes;
    Reset();
    num_nodes = nodes.size();
    real_nodes = nodes.size();
    
    CalculateDimension();
    CalculateConnections();
    
    // TODO: The Connections are not that good on small not fully filled cubes
    
    // Each dimension must be connected
    for (dim = 0; dim < dimension; dim++) {
      // this num nodes we have to connect and then to step over, so 2*step
      step = std::pow(2, dim);
      for (i = 0; i < num_nodes; i+=(2 * step)) {
        // connect nodes with the one "step" positions after the current
        for (j = 0; j < step; j++) {
          // Only connect if we really have a node
          if ((i+j) < real_nodes) {
            nodes.at(i+j)->ConnectNode( nodes.at( (i+j+step) % real_nodes ) );
          }
        }
      }
    }
  }
  
  void Hypercube::debug() {
    std::cout << "Hypercube:\n  Dimension: "<< Dimension() << "\n  Nodes: " << num_nodes << " (Real " << GetNumNodes() << ")\n  Connections: "<< Connections() << "\n\n";
    int i;
    for (i = 0; i < nodes.size(); i++) {
      nodes.at(i)->debug();
    }
  }




  HyperNode::HyperNode() {
    network = NULL;
    Identifier(lukyluke::helper::GetGuid());
  }
  
  HyperNode::HyperNode(std::string label) {
    network = NULL;
    Identifier(lukyluke::helper::GetGuid());
    Label(label);
  }
  
  HyperNode::HyperNode(std::string label, std::string host) {
    network = NULL;
    Identifier(lukyluke::helper::GetGuid());
    Label(label);
    Host(host);
  }
  
  HyperNode::HyperNode(std::string label, Hypercube* netw) {
    network = netw;
    Identifier(lukyluke::helper::GetGuid());
    Label(label);
    Host("");
  }
  
  HyperNode::~HyperNode() {
    ClearConnections();
  }

  void HyperNode::ConnectNode(HyperNode* node) {
    if (isConnected(node) || (node->Identifier() == Identifier())) {
      return;
    }
    nodes.push_back(node);
    node->ConnectNode(this);
  }
  
  bool HyperNode::isConnected(HyperNode* node) {
    int i;
    std::string id = node->Identifier();
    for (i = 0; i < nodes.size(); i++) {
      if (nodes[i]->Identifier() == id) {
        return true;
      }
    }
    return false;
  }
  
  void HyperNode::ClearConnections() {
    // No need to destroy the other nodes, just clear the Connections to them
    nodes.clear();
  }
  
  std::vector<HyperNode*>* HyperNode::GetConnectedNodes() {
    return &nodes;
  }

  void HyperNode::debug() {
    std::cout << "HyperNode\n  Label: "<< Label() << "\n  ID: "<< Identifier() << "\n  Connections: " << NumConnections() << " (";
    int i;
    for (i = 0; i < nodes.size(); i++) {
      std::cout << nodes.at(i)->Label() << ",";
    }
    std::cout << ")\n";
    
    if (network != NULL) {
      network->debug();
    }
  }

}