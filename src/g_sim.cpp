#include <iostream>
#include <vector>
#include <queue>
#include <list>

// Process Modules
#include "module.h"
#include "memory.h"
#include "crossbar.h"
#include "readSrcProperty.h"
#include "readSrcEdges.h"
#include "readDstProperty.h"
#include "controlAtomicUpdate.h"
#include "processEdge.h"
#include "readTempDstProperty.h"
#include "reduce.h"
#include "writeTempDstProperty.h"

// Apply Modules
#include "apply.h"
#include "readVertexProperty.h"
#include "readTempVertexProperty.h"
#include "writeVertexProperty.h"
#include "readGraph.h"

// Utility
#include "option.h"
#include "edge.h"

// GraphMat
#include "bfs.h"

#define ITERATIONS 10000

// The vertex type
typedef bool vertex_t;

// The edge type
typedef double edge_t;

void print_queue(std::string name, std::list<uint64_t>* q, int iteration) {
  std::cerr << "Iteration: " << iteration << " " << name << " Queue Size " << q->size();
  std::cerr << "   " << name << " Queue: [ ";
  int i = 0;
  for(auto it = q->begin(); it != q->end() && i < 20; it++) {
    std::cerr << *it << ", ";
    i++;
  }
  if(i < 20) {
    std::cerr << "]\n";
  }
  else {
    std::cerr << "...\n";
  }
}

int main(int argc, char** argv) {
  Utility::Options opt;
  opt.parse(argc, argv);
  Utility::readGraph<vertex_t> graph(opt);
  graph.setInitializer(false);
  graph.readMatrixMarket(opt.graph_path.c_str());
#ifdef DEBUG
  //graph.printGraph();
#endif

  GraphMat::BFS<vertex_t, edge_t> bfs;

  std::list<uint64_t>* process = new std::list<uint64_t>;

  uint64_t global_tick = 0;

  // Setup problem:
  process->push_back(1);
  graph.setVertexProperty(1, true);

  // Iteration Loop:
  for(uint64_t iteration = 0; iteration < opt.num_iter && !process->empty(); iteration++) {
#ifdef DEBUG
    //print_queue("Process", process, iteration);
    //graph.printVertexProperties();
#endif
    // Processing Phase 
    p1.ready();
    while(!p8.complete()) {
      global_tick++;
      mem.tick();
    }
    p8.flush();
#ifdef DEBUG
    print_queue("Apply", apply, iteration);
#endif
    
    // Apply Phase
    a1.ready();
    while(!a4.complete()) {
      global_tick++;
      mem.tick();
    }
    a4.flush();
  }
#ifdef DEBUG
  p8.print_stats();
  graph.printVertexProperties(30);
  std::cout << global_tick << "\n";
#endif

  return 0;
}
