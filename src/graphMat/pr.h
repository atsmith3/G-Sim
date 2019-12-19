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

#define ALPHA 0.85f
#define TOLERANCE 1.0e-3f

class pr_t {
public:
  double delta;
  double residual;
  double pageRank;
  pr_t() {
    delta = 0.0;
    residual = 1-ALPHA;
    pageRank = 0.0;
  }
  friend std::ostream& operator<<(std::ostream& os, const pr_t& obj) {
    os << obj.pageRank << "," << obj.delta << "," << obj.residual;
    return os;
  }
  bool operator==(const pr_t& obj) const {
    return this->delta == obj.delta && this->residual == obj.residual && this->pageRank == obj.pageRank;
  }
  bool operator!=(const pr_t& obj) const {
    return !(this->delta == obj.delta && this->residual == obj.residual && this->pageRank == obj.pageRank);
  }
};

template<class v_t, class e_t>
class PR : public GraphApp<v_t, e_t> {
protected:

public:
  // Constructor
  PR() {}

  // Destructor
  ~PR() { /* Do Nothing */ }

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

}; // class PR

}; // namespace GraphMat

#include "pr.tcc"

#endif
