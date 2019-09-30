#include <iostream>
#include <vector>
#include <queue>
#include <list>

// Process Modules
#include "module.h"
#include "memory.h"
#include "crossbar.h"
#include "readSrcProperty.h"
//#include "readSrcEdges.h"
//#include "readDstProperty.h"
//#include "controlAtomicUpdate.h"
//#include "processEdge.h"
//#include "readTempDstProperty.h"
//#include "reduce.h"
//#include "writeTempDstProperty.h"
//
//// Apply Modules
//#include "apply.h"
//#include "readVertexProperty.h"
//#include "readTempVertexProperty.h"
//#include "writeVertexProperty.h"
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
  graph.readMatrixMarket(opt.graph_path.c_str());

  GraphMat::BFS<vertex_t, edge_t> bfs;

  std::queue<uint64_t>* process = new std::queue<uint64_t>;
  std::queue<uint64_t>* apply = new std::queue<uint64_t>;

  // "Scratchpad memory"
  std::map<uint64_t, vertex_t>* scratchpad_map = new std::map<uint64_t, vertex_t>;

  uint64_t global_tick = 0;

  // Setup problem:
  process->push(1);
  graph.setVertexProperty(1, true);

  // Iteration Loop:
  for(uint64_t iteration = 0; iteration < opt.num_iter; i++) {
    // Processing Phase 
    while(!process->empty()) {
      global_tick++;




    }
  std::queue<uint64_t>* next_frontier = new std::queue<uint64_t>;
    
    // Apply Phase
    while(!apply->empty()) {
      global_tick++;



    }
  }

  return 0;

  // Processing Phase Pipeline:
  SimObj::Memory mem(opt.dram_read_latency, opt.dram_write_latency, opt.dram_num_simultaneous_requests);
  SimObj::Memory scratchpad(opt.scratchpad_read_latency, opt.scratchpad_write_latency, opt.scratchpad_num_simultaneous_requests);

  SimObj::ReadSrcProperty<vertex_t, edge_t> p1(&mem, process, &graph);
  SimObj::ReadSrcEdges p2(&scratchpad, opt.avg_connectivity);
  SimObj::ReadDstProperty p3(&mem);
  SimObj::ProcessEdge p4(1);
  SimObj::ControlAtomicUpdate p5;
  SimObj::ReadTempDstProperty p6(&scratchpad);
  SimObj::Reduce p7(1);
  SimObj::WriteTempDstProperty p8(&scratchpad, &p5);

  SimObj::Memory mem_a(opt.dram_read_latency, opt.dram_write_latency, opt.dram_num_simultaneous_requests);
  SimObj::Memory scratchpad_a(opt.scratchpad_read_latency, opt.scratchpad_write_latency, opt.scratchpad_num_simultaneous_requests);
  SimObj::ReadVertexProperty a1(&mem_a);
  SimObj::ReadTempVertexProperty a2(&scratchpad_a);
  SimObj::Apply a3;
  SimObj::WriteVertexProperty a4(&mem_a);

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
  
  p1.set_name("ReadSrcProperty");
  p2.set_name("ReadSrcEdges");
  p3.set_name("ReadDstProperty");
  p4.set_name("ProcessEdge");
  p5.set_name("ControlAtomicUpdate");
  p6.set_name("ReadTempDstProperty");
  p7.set_name("Reduce");
  p8.set_name("WriteTempDstProperty");

  for(int i = 0; i < ITERATIONS; i++) {
    mem.tick();
    scratchpad.tick();
    p1.tick();
    p2.tick();
    p3.tick();
    p4.tick();
    p5.tick();
    p6.tick();
    p7.tick();
    p8.tick();
  }

  p1.print_stats_csv();
  p2.print_stats_csv();
  p3.print_stats_csv();
  p4.print_stats_csv();
  p5.print_stats_csv();
  p6.print_stats_csv();
  p7.print_stats_csv();
  p8.print_stats_csv();

  a1.set_prev(NULL);
  a1.set_next(&a2);
  a2.set_prev(&a1);
  a2.set_next(&a3);
  a3.set_prev(&a2);
  a3.set_next(&a4);
  a4.set_prev(&a3);
  a4.set_next(NULL);

  a1.set_name("ReadVertexProperty");
  a2.set_name("ReadTempVertexProperty");
  a3.set_name("Apply");
  a4.set_name("WriteVertexProperty");

  for(int i = 0; i < ITERATIONS; i++) {
    mem_a.tick();
    scratchpad_a.tick();
    a1.tick();
    a2.tick();
    a3.tick();
    a4.tick();
  }

  a1.print_stats_csv();
  a2.print_stats_csv();
  a3.print_stats_csv();
  a4.print_stats_csv();
}
