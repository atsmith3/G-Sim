/*
 *
 * Andrew Smith
 *
 * BFS graphMat implementation for Graphicionado
 *
 */

#ifndef GRAPHMAT_BFS_H
#define GRAPHMAT_BFS_H

#include "graphMat.h"

namespace GraphMat {

template<class vertex_t, class edge_t>
class BFS : public GraphApp<vertex_t, edge_t> {
protected:
  

public:
  // Constructor
  BFS() {}

  // Destructor
  ~BFS() { /* Do Nothing */ }

  // Reduction Function
  void reduce(vertex_t& a, const vertex_t& b);

  // Process Edge Function
  void process_edge(vertex_t& message, const edge_t& edge, const vertex_t& vertex);

  // Apply
  void apply(const vertex_t& scratch, vertex_t& dram);

}; // class BFS

}; // namespace GraphMat

#include "bfs.tcc"

#endif
