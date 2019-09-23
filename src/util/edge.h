/*
 * Andrew Smith
 *
 * Template Class for Edge
 *
 */

#ifndef EDGE_H
#define EDGE_H

#include <string>

namespace Utility {

template<class edge_t>
class Edge {
private:
  uint64_t src;
  uint64_t dst;
  edge_t edge_property;

public:
  // Constructor
  Edge(uint64_t src, uint64_t dst, edge_t prop);

  // Destructor
  ~Edge();

  // Get Methods
  edge_t get_property() const;
  uint64_t get_src() const;
  uint64_t get_dst() const;

  // Set Methods
  void set_property(edge_t property);
}; // class Edge

#include "edge.tcc"

}; // namespace Utility


#endif // EDGE_H
