/*
 *
 * Andrew Smith
 *
 * Single Graphicionado Pipeline, can be connected together via the crossbar module.
 * Each pipeline get its own Scratchpad
 *
 * 10/07/19
 *
 */

#ifndef PIPELINE_H
#define PIPELINE_H

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

namespace SimObj {

template<class v_t, class e_t>
class Pipeline {
private:
  std::list<uint64_t>* apply;
  std::list<uint64_t>* process;
  Crossbar<v_t, e_t>* crossbar;
  std::map<uint64_t, Utility::pipeline_data<v_t, e_t>>* scratchpad_map;

  SimObj::ReadSrcProperty<vertex_t, edge_t>* p1(&mem, process, &graph);
  SimObj::ReadSrcEdges<vertex_t, edge_t>* p2(&scratchpad, &graph);
  SimObj::ReadDstProperty<vertex_t, edge_t>* p3(&mem, &graph);
  SimObj::ProcessEdge<vertex_t, edge_t>* p4(1, &bfs);
  SimObj::ControlAtomicUpdate<vertex_t, edge_t>* p5;
  SimObj::ReadTempDstProperty<vertex_t, edge_t>* p6(&scratchpad, &graph, scratchpad_map);
  SimObj::Reduce<vertex_t, edge_t>* p7(1, &bfs);
  SimObj::WriteTempDstProperty<vertex_t, edge_t>* p8(&scratchpad, &p5, scratchpad_map, apply);

  SimObj::ReadVertexProperty<vertex_t, edge_t>* a1(&mem, apply, &graph);
  SimObj::ReadTempVertexProperty<vertex_t, edge_t>* a2(&scratchpad, &graph, scratchpad_map);
  SimObj::Apply<vertex_t, edge_t>* a3(1, &bfs);
  SimObj::WriteVertexProperty<vertex_t, edge_t>* a4(&mem, process, &graph);


  uint64_t _tick;
  bool process_complete;
  bool apply_complete;

public:
  // Constructor:
  Pipeline(uint64_t pipeline_id, const Utility::Options opt, Utility::readGraph<v_t>* graph, GraphMat::GraphApp<v_t, e_t>* application, Memory* mem, Crossbar<v_t, e_t>* crossbar);

  // Destructor:
  ~Pipeline();


  // Methods:
  void tick_process();
  void tick_apply();

  bool process_complete();
  bool apply_complete();

  void flush_process();
  void flush_apply();

  void print_stats();

}; // class Pipeline

}; // namespace SimObj

#include "pipeline.tcc"

#endif
