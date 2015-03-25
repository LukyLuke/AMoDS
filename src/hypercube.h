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

#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include <stdint.h>
#include <string>
#include <vector>
#include "helper/guid.h"

namespace amods {

	class Hypercube;
	class HyperNode;

	class Hypercube
	{
		private:
			uint32_t dimension;
			uint32_t num_nodes;
			uint32_t num_connections;
			std::vector<HyperNode*> nodes;
			void ClearNodes();
			
		protected:
			void CalculateNodes();
			void CalculateConnections();
			void CalculateDimension();
			
		public:
			Hypercube();
			Hypercube(uint32_t dimension);
			virtual ~Hypercube();
			uint32_t Dimension() { return dimension; };
			void Dimension(uint32_t dim) { dimension = dim; };
			uint32_t Connections() { return num_connections; };
			void Connections(uint32_t conn) { num_connections = conn; };
			std::vector<HyperNode*>* GetNodes();
			uint32_t GetNumNodes() { return nodes.size(); };
			void BuildFromNodes(std::vector<HyperNode*>* new_nodes);
			void AddNode(HyperNode* node);
			void AddNodes(std::vector<HyperNode*>* nodes);
			void Reset();
			void BuildHypercube();
			void debug();
	};
	
	
	class HyperNode
	{
		private:
			std::string identifier;
			std::string label;
			std::string address;
			Hypercube* network;
			
		protected:
			std::vector<HyperNode*> nodes;
			void Identifier(std::string id) { identifier = id; };
			
		public:
			HyperNode();
			HyperNode(std::string label);
			HyperNode(std::string label, std::string host);
			HyperNode(std::string label, Hypercube* netw);
			virtual ~HyperNode();
			std::string Identifier() { return identifier; };
			std::string Label() { return label; };
			void Label(std::string l) { label = l; };
			std::string Host() { return address; };
			void Host(std::string host) { address = host; };
			void Network(Hypercube* netw) { network = netw; };
			Hypercube* Network() { return network; };
			uint32_t NumConnections() { return nodes.size(); };
			void ClearConnections();
			void ConnectNode(HyperNode* node);
			bool isConnected(HyperNode* node);
			std::vector<HyperNode*>* GetConnectedNodes();
			void debug();
	};
}
#endif // HYPERCUBE_H
