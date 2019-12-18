// Andrew Smith
//
// Graph class
//
// 121219

#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>
#include <vector>
#include <list>

namespace Utility {

template<class e_t>
class Edge {
public:
  e_t property;
  uint64_t dst;
  uint64_t src;

  Edge(e_t init, uint64_t src, uint64_t dst);
};

template<class v_t, class e_t>
class Vertex {
public:
  v_t property;
  std::vector<Edge<e_t>> edges;
  std::vector<Edge<e_t>> in_edges;

  template<class T, class U>
  friend std::ostream& operator<<(std::ostream& os, const Vertex<T, U>& obj);
};

template<class v_t, class e_t>
class Graph {
public:
  std::string graph_name;
  std::vector<Vertex<v_t, e_t>> vertex;

  void import(std::string fname);
  void printVertexProperties(uint64_t n);
  void writeVertexProperties(std::string fname);
  void serialize();
  void deserialize();
};

}; // namespace Utility

#include "graph.tcc"

#endif // GRAPH_H
