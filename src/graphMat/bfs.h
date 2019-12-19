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

class bfs_t {
public:
  bool visited;
  bfs_t() {
    visited = false;
  }
  friend std::ostream& operator<<(std::ostream& os, const bfs_t& obj) {
    os << obj.visited;
    return os;
  }
  bool operator==(const bfs_t& obj) const {
    return obj.visited == this->visited;
  }
  bool operator!=(const bfs_t& obj) const {
    return obj.visited != this->visited;
  }
  bool operator<(const bfs_t& obj) const {
    return this->visited < obj.visited;
  }
  bool operator>(const bfs_t& obj) const {
    return this->visited >= obj.visited;
  }
};

template<class v_t, class e_t>
class BFS : public GraphApp<v_t, e_t> {
protected:
  

public:
  // Constructor
  BFS() {}

  // Destructor
  ~BFS() { /* Do Nothing */ }

  // Init
  void initialize(Utility::Graph<v_t, e_t>& graph, std::list<uint64_t>* curr); 

  // Do Every Iteration
  void do_every_iteration(Utility::Graph<v_t, e_t>& graph, std::list<uint64_t>* curr); 

  // Reduction Function
  bool reduce(v_t& a, const v_t& b);

  // Process Edge Function
  void process_edge(v_t& message, const e_t& edge, const v_t& vertex);

  // Apply
  bool apply(const v_t& scratch, v_t& dram);
}; // class BFS

}; // namespace GraphMat

#include "bfs.tcc"

#endif
