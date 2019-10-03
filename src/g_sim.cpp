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

int main(int argc, char** argv) {
  Utility::Options opt;
  opt.parse(argc, argv);
  Utility::readGraph<vertex_t> graph(opt);
  graph.setInitializer(false);
  graph.readMatrixMarket(opt.graph_path.c_str());
  graph.printGraph();

  GraphMat::BFS<vertex_t, edge_t> bfs;

  std::queue<uint64_t>* process = new std::queue<uint64_t>;
  std::queue<uint64_t>* apply = new std::queue<uint64_t>;

  // "Scratchpad memory"
  std::map<uint64_t, Utility::pipeline_data<vertex_t, edge_t>>* scratchpad_map = new std::map<uint64_t, Utility::pipeline_data<vertex_t, edge_t>>;

  uint64_t global_tick = 0;

  // Setup problem:
  process->push(0);
  graph.setVertexProperty(0, true);

  // Pipeline Modules
  SimObj::Memory mem(opt.dram_read_latency, opt.dram_write_latency, opt.dram_num_simultaneous_requests);
  SimObj::Memory scratchpad(opt.scratchpad_read_latency, opt.scratchpad_write_latency, opt.scratchpad_num_simultaneous_requests);

  SimObj::ReadSrcProperty<vertex_t, edge_t> p1(&mem, process, &graph);
  SimObj::ReadSrcEdges<vertex_t, edge_t> p2(&scratchpad, &graph);
  SimObj::ReadDstProperty<vertex_t, edge_t> p3(&mem, &graph);
  SimObj::ProcessEdge<vertex_t, edge_t> p4(1, &bfs);
  SimObj::ControlAtomicUpdate<vertex_t, edge_t> p5;
  SimObj::ReadTempDstProperty<vertex_t, edge_t> p6(&scratchpad, &graph, scratchpad_map);
  SimObj::Reduce<vertex_t, edge_t> p7(1, &bfs);
  SimObj::WriteTempDstProperty<vertex_t, edge_t> p8(&scratchpad, &p5, scratchpad_map, apply);

  SimObj::ReadVertexProperty<vertex_t, edge_t> a1(&mem, apply, &graph);
  SimObj::ReadTempVertexProperty<vertex_t, edge_t> a2(&scratchpad, &graph, scratchpad_map);
  SimObj::Apply<vertex_t, edge_t> a3(1, &bfs);
  SimObj::WriteVertexProperty<vertex_t, edge_t> a4(&mem, process, &graph);
  
  // Connect:
  p1.set_next(&p2);
  p1.set_prev(NULL);
  p2.set_next(&p3);
  p2.set_prev(&p1);
  p3.set_next(&p4);
  p3.set_prev(&p2);
  p4.set_next(&p5);
  p4.set_prev(&p3);
  p5.set_next(&p6);
  p5.set_prev(&p4);
  p6.set_next(&p7);
  p6.set_prev(&p5);
  p7.set_next(&p8);
  p7.set_prev(&p6);
  p8.set_next(NULL);
  p8.set_prev(&p7);

  a1.set_prev(NULL);
  a1.set_next(&a2);
  a2.set_prev(&a1);
  a2.set_next(&a3);
  a3.set_prev(&a2);
  a3.set_next(&a4);
  a4.set_prev(&a3);
  a4.set_next(NULL);

  // Name
  p1.set_name("ReadSrcProperty");
  p2.set_name("ReadSrcEdges");
  p3.set_name("ReadDstProperty");
  p4.set_name("ProcessEdge");
  p5.set_name("ControlAtomicUpdate");
  p6.set_name("ReadTempDstProperty");
  p7.set_name("Reduce");
  p8.set_name("WriteTempDstProperty");

  a1.set_name("ReadVertexProperty");
  a2.set_name("ReadTempVertexProperty");
  a3.set_name("Apply");
  a4.set_name("WriteVertexProperty");

  // Iteration Loop:
  for(uint64_t iteration = 0; iteration < opt.num_iter && !process->empty(); iteration++) {
    std::cout << "Iteration: " << iteration << " Process Queue Size " << process->size() << "\n";
    // Processing Phase 
    while(!p8.complete()) {
      global_tick++;
      p1.tick();
      p2.tick();
      p3.tick();
      p4.tick();
      p5.tick();
      p6.tick();
      p7.tick();
      p8.tick();
      mem.tick();
      scratchpad.tick();
    }
    p8.flush();
    
    // Apply Phase
    while(!a4.complete()) {
      global_tick++;
      a1.tick();
      a2.tick();
      a3.tick();
      a4.tick();
      mem.tick();
      scratchpad.tick();
    }
    a4.flush();
  }

  return 0;
}
