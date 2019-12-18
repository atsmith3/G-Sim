/*
 *
 * Andrew Smith
 *
 * SSSP graphMat implementation for Graphicionado
 *
 * Single Source Shortest Path (SSSP) finds the shortest distance from 
 * the start node to all of the nodes reachable from the start node.
 *
 */

#ifndef GRAPHMAT_SSSP_H
#define GRAPHMAT_SSSP_H

#include "graphMat.h"

namespace GraphMat {

template<class v_t, class e_t>
class SSSP : public GraphApp<v_t, e_t> {
protected:
  

public:
  // Constructor
  SSSP() {}

  // Destructor
  ~SSSP() { /* Do Nothing */ }

  // Init
  void initialize(Utility::Graph<v_t, e_t>& graph, std::list<uint64_t>* curr);

  // Function to do every iteration of the graph application
  void do_every_iteration(Utility::Graph<v_t, e_t>& graph, std::list<uint64_t>* curr);

  // Reduction Function
  bool reduce(v_t& a, const v_t& b);

  // Process Edge Function
  void process_edge(v_t& message, const e_t& edge, const v_t& vertex);

  // Apply
  bool apply(const v_t& scratch, v_t& dram);

}; // class SSSP

}; // namespace GraphMat

#include "sssp.tcc"

#endif
