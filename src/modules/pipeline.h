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
  SimObj::Memory* scratchpad;

  SimObj::ReadSrcProperty<v_t, e_t>* p1;
  SimObj::ReadSrcEdges<v_t, e_t>* p2;
  SimObj::ReadDstProperty<v_t, e_t>* p3;
  SimObj::ProcessEdge<v_t, e_t>* p4;
  SimObj::ControlAtomicUpdate<v_t, e_t>* p5;
  SimObj::ReadTempDstProperty<v_t, e_t>* p6;
  SimObj::Reduce<v_t, e_t>* p7;
  SimObj::WriteTempDstProperty<v_t, e_t>* p8;

  SimObj::ReadVertexProperty<v_t, e_t>* a1;
  SimObj::ReadTempVertexProperty<v_t, e_t>* a2;
  SimObj::Apply<v_t, e_t>* a3;
  SimObj::WriteVertexProperty<v_t, e_t>* a4;


  uint64_t _tick;

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

  void process_ready();
  void apply_ready();

  void print_stats();

}; // class Pipeline

}; // namespace SimObj

#include "pipeline.tcc"

#endif
