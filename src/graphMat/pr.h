/*
 *
 * Andrew Smith
 *
 * PR graphMat implementation for Graphicionado
 *
 * "The Page Rank (PR) algorithm calculates scores of vertices
 * in a graph based on some metric (e.g., popularity). Web pages are 
 * represented as vertices and hyperlinks are represented as edges. 
 * The equation below shows how the PageRank score is calculated for each 
 * vertex. α is a constant and Udeg is the out-degree and a constant 
 * property of vertex U. In PageRank, all vertices are considered active
 * in all iterations." 
 * -https://mrmgroup.cs.princeton.edu/papers/taejun_micro16.pdf
 * 
 */

#ifndef GRAPHMAT_PR_H
#define GRAPHMAT_PR_H

#include "graphMat.h"

namespace GraphMat {

template<class v_t, class e_t>
class PR : public GraphApp<v_t, e_t> {
protected:
  double alpha;
  double tolerance;
  

public:
  // Constructor
  PR(double alpha, double tolerance) {
    this->alpha = alpha;
    this->tolerance = alpha;
  }

  // Destructor
  ~PR() { /* Do Nothing */ }

  // Reduction Function
  bool reduce(v_t& a, const v_t& b);

  // Process Edge Function
  void process_edge(v_t& message, const e_t& edge, const v_t& vertex);

  // Apply
  bool apply(const v_t& scratch, v_t& dram);

}; // class PR

}; // namespace GraphMat

#include "pr.tcc"

#endif
