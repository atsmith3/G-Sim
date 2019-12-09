/*
 * Andrew Smith
 *
 * Read SRC Edges:
 *  The Read SRC Edges Module is the second module in the pipeline.
 *  It reads the list of edges of the current vertex from Scratchpad Memory.
 *
 */

#ifndef READSRCEDGES_H
#define READSRCEDGES_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>
#include <queue>

#include "module.h"
#include "memory.h"

#include "readGraph.h"

namespace SimObj {

template<class v_t, class e_t>
class ReadSrcEdges : public Module<v_t, e_t> {
private:
  enum op_t {
    OP_WAIT,
    OP_MEM_WAIT,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_MEM_WAIT"},
    {2, "OP_NUM_OPS"}};
#endif

  using Module<v_t, e_t>::_tick;
  using Module<v_t, e_t>::_ready;
  using Module<v_t, e_t>::_stall;
  using Module<v_t, e_t>::_next;
  using Module<v_t, e_t>::_data;
  using Module<v_t, e_t>::_name;
  using Module<v_t, e_t>::_has_work;
#if MODULE_TRACE
  using Module<v_t, e_t>::_in_data;
  using Module<v_t, e_t>::_in_logger;
  using Module<v_t, e_t>::_out_logger;
  bool ready_prev;
  bool mem_flag_prev;
  bool send_prev;
  bool edges_empty_prev;
  e_t edge_data_prev;
  uint64_t dst_id_prev;
  uint64_t address_prev;
  uint64_t num_edges_prev;
  
  bool ready_curr;
  bool mem_flag_curr;
  bool send_curr;
  bool edges_empty_curr;
  e_t edge_data_curr;
  uint64_t dst_id_curr;
  uint64_t address_curr;
  uint64_t num_edges_curr;

  void update_logger();
#endif

  Memory* _scratchpad;
  op_t _state;
  std::queue<uint>* _edge_list;
  Utility::readGraph<v_t>* _graph;
  bool _data_set;
  uint64_t _id;
  uint64_t _curr_addr;

public:
  bool _mem_flag;
  ReadSrcEdges(Memory* dram, Utility::readGraph<v_t>* graph, std::string name, uint64_t id);
  ~ReadSrcEdges();

  void tick(void);
  void ready(Utility::pipeline_data<v_t, e_t> data);
};

} // namespace SimObj

#include "readSrcEdges.tcc"

#endif
