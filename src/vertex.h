/*
 * Andrew Smith
 * 
 * Vertex Class
 *
 */

#ifndef VERTEX_H
#define VERTEX_H

#include <vector>
#include "edge.h"

namespace Utility {

template<class vertex_t, class edge_t>
class Vertex {
public:
  std::vector<Edge<edge_t>> edge_list;
  vertex_t property;
  uint64_t id;
  uint64_t curr_edge;

  // Constructor
  Vertex(uint64_t vertex_id, vertex_t vertex_property);
  
  // Destructor
  ~Vertex();
}; // class Vertex

#include "vertex.tcc"

}; // namespace Utility

#endif // VERTEX_H
