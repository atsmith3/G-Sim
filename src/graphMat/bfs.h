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

template<class v_t, class e_t>
class BFS : public GraphApp<v_t, e_t> {
protected:
  

public:
  // Constructor
  BFS() {}

  // Destructor
  ~BFS() { /* Do Nothing */ }

  // Reduction Function
  void reduce(v_t& a, const v_t& b);

  // Process Edge Function
  void process_edge(v_t& message, const e_t& edge, const v_t& vertex);

  // Apply
  void apply(const v_t& scratch, v_t& dram);

}; // class BFS

}; // namespace GraphMat

#include "bfs.tcc"

#endif
