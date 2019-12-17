/*
 *
 * Andrew Smith
 *
 * CC graphMat implementation for Graphicionado
 *
 * Connected Components labels each vertex with an integer
 * corresponding to the set of verticies reachable from that
 * vertex.
 *
 */

#ifndef GRAPHMAT_CC_H
#define GRAPHMAT_CC_H

#include "graphMat.h"

namespace GraphMat {

template<class v_t, class e_t>
class CC : public GraphApp<v_t, e_t> {
protected:
  

public:
  // Constructor
  CC() {}

  // Destructor
  ~CC() { /* Do Nothing */ }

  // Reduction Function
  bool reduce(v_t& a, const v_t& b);

  // Process Edge Function
  void process_edge(v_t& message, const e_t& edge, const v_t& vertex);

  // Apply
  bool apply(const v_t& scratch, v_t& dram);

}; // class CC

}; // namespace GraphMat

#include "cc.tcc"

#endif
