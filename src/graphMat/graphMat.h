/*
 * Andrew Smith
 * 
 * Parent class for the GraphMat API
 *
 */

#ifndef GRAPHMAT_H
#define GRAPHMAT_H

#include "graph.h"

namespace GraphMat {

template<class v_t, class e_t>
class GraphApp {
protected:
  uint64_t iteration;

public:
  // Constructor
  GraphApp() { iteration = 0; }

  // Destructor
  ~GraphApp() { /* Do nothing */ }

  // Init
  virtual void initialize(Utility::Graph<v_t, e_t>& graph, std::list<uint64_t>* curr);

  // Function to do every iteration of the graph application
  virtual void do_every_iteration(Utility::Graph<v_t, e_t>& graph, std::list<uint64_t>* curr);

  // Applies reduction function to a vertex to be written to scratchpad
  virtual bool reduce(v_t& a, const v_t& b);

  // Processes edge
  virtual void process_edge(v_t& message, const e_t& edge, const v_t& vertex);

  // during the apply phase will update the global memory
  virtual bool apply(const v_t& scratch, v_t& dram);


}; // class GraphApp

}; //namespace GraphMat

#include "graphMat.tcc"

#endif // GRAPHMAT_H
