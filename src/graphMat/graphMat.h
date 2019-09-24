/*
 * Andrew Smith
 * 
 * Parent class for the GraphMat API
 *
 */

#ifndef GRAPHMAT_H
#define GRAPHMAT_H

namespace GraphMat {

template<class vertex_t, class edge_t>
class GraphApp {
protected:
  uint64_t iteration;

public:
  // Constructor
  GraphApp() { iteration = 0; }

  // Destructor
  ~GraphApp() { /* Do nothing */ }

  // Applies reduction function to a vertex to be written to scratchpad
  virtual void reduce(vertex_t& a, const vertex_t& b);

  // Processes edge
  virtual void process_edge(vertex_t& message, const edge_t& edge, const vertex_t& vertex);

  // during the apply phase will update the global memory
  virtual void apply(const vertex_t& scratch, vertex_t& dram);

  // Function to do every iteration of the graph application
  virtual void do_every_iteration();

}; // class GraphApp

}; //namespace GraphMat

#include "graphMat.tcc"

#endif // GRAPHMAT_H
