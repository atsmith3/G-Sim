#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <list>

// Process Modules
#include "memory.h"
#include "dram.h"
#include "crossbar.h"

// Pipeline Class
#include "pipeline.h"

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
  std::vector<SimObj::Pipeline<vertex_t, edge_t>*>* tile = new std::vector<SimObj::Pipeline<vertex_t, edge_t>*>;

  SimObj::Crossbar<vertex_t, edge_t>* crossbar = new SimObj::Crossbar<vertex_t, edge_t>(opt.num_pipelines);
  SimObj::Memory* mem = new SimObj::DRAM;

  for(uint64_t i = 0; i < opt.num_pipelines; i++) {
    SimObj::Pipeline<vertex_t, edge_t>* temp = new SimObj::Pipeline<vertex_t, edge_t>(i, opt, &graph, process, &bfs, mem, crossbar);
    tile->push_back(temp);
  }

  uint64_t global_tick = 0;
  bool complete = false;
  uint64_t edges_processed = 0;

  // Setup problem:
  process->push_back(1);
  graph.setVertexProperty(1, true);

  // Iteration Loop:
  for(uint64_t iteration = 0; iteration < opt.num_iter && !process->empty(); iteration++) {
#ifdef DEBUG
    print_queue("Process", process, iteration);
    //graph.printVertexProperties();
#endif
    // Processing Phase 
    std::for_each(tile->begin(), tile->end(), [](SimObj::Pipeline<vertex_t, edge_t>* a) {a->process_ready();});
    complete = false;
    while(!complete) {
      global_tick++;
      std::for_each(tile->begin(), tile->end(), [](SimObj::Pipeline<vertex_t, edge_t>* a) {a->tick_process();});
      std::for_each(tile->begin(), tile->end(), [](SimObj::Pipeline<vertex_t, edge_t>* a) {a->print_debug();});
      crossbar->tick();
      mem->tick();
      complete = true;
      std::for_each(tile->begin(), tile->end(), [&complete, crossbar](SimObj::Pipeline<vertex_t, edge_t>* a) mutable {
        if(!a->process_complete() || crossbar->busy()) complete = false;
      });
    }
#ifdef DEBUG
    //print_queue("Apply", apply, iteration);
#endif

    // Accumulate the edges processed each iteration
    std::for_each(tile->begin(), tile->end(), [&edges_processed](SimObj::Pipeline<vertex_t, edge_t>* a) mutable {
      edges_processed += a->apply_size();
    });
    
    // Apply Phase
    std::for_each(tile->begin(), tile->end(), [](SimObj::Pipeline<vertex_t, edge_t>* a) {a->apply_ready();});
    complete = false;
    while(!complete) {
      global_tick++;
      std::for_each(tile->begin(), tile->end(), [](SimObj::Pipeline<vertex_t, edge_t>* a) {a->tick_apply();});
      //std::for_each(tile->begin(), tile->end(), [](SimObj::Pipeline<vertex_t, edge_t>* a) {a->print_debug();});
      mem->tick();
      complete = true;
      std::for_each(tile->begin(), tile->end(), [&complete](SimObj::Pipeline<vertex_t, edge_t>* a) mutable {
        if(!a->apply_complete()) complete = false;
      });
    }
  }
#ifdef DEBUG
  graph.printVertexProperties(30);
  std::cout << "Global Ticks, " << global_tick << ", Edges Processed, " << edges_processed << ", Throughput (Edges/Cycle), " << (float)edges_processed/(float)global_tick << "\n";
#endif

  for(uint64_t i = 0; i < opt.num_pipelines; i++) {
    delete tile->operator[](i);
  }

  graph.writeVertexPropertyToFile(opt.result);

  return 0;
}
