/*
 * Andrew Smith
 *
 * Read SRC Property:
 *  This is the frontend module of the graphicionado accelerator;
 *  it performs a sequential read on the value of the src vertex.
 *
 *  It has to interface with the DRAM memory to fetch the active
 *  property. Edges are stored in the Scratchpad.
 *
 */

#ifndef READSRCPROPERTY_H
#define READSRCPROPERTY_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>
#include <list>

#include "module.h"
#include "memory.h"

#include "graph.h"

namespace SimObj {

template<class v_t, class e_t>
class ReadSrcProperty : public Module<v_t, e_t> {
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
  using Module<v_t, e_t>::_name;
  using Module<v_t, e_t>::_next;
  using Module<v_t, e_t>::_data;
  using Module<v_t, e_t>::_has_work;
  using Module<v_t, e_t>::_items_processed;
#if MODULE_TRACE
  using Module<v_t, e_t>::_in_data;
  using Module<v_t, e_t>::_in_logger;
  using Module<v_t, e_t>::_out_logger;
  bool ready_prev;
  bool mem_flag_prev;
  bool send_prev;
  uint64_t address_prev;
  v_t mem_result_prev;
  uint64_t queue_length_prev;
  bool iteration_reset_prev;
  bool ready_curr;
  bool mem_flag_curr;
  bool send_curr;
  uint64_t address_curr;
  v_t mem_result_curr;
  uint64_t queue_length_curr;
  bool iteration_reset_curr;

  void update_logger();
#endif

  Memory* _dram;
  op_t _state;
  bool _fetched;
  std::list<uint64_t>* _process;
  Utility::Graph<v_t, e_t>* _graph;
  uint64_t _base_addr;
  uint64_t _curr_addr;
  uint64_t _id;

public:
  bool _mem_flag;
  ReadSrcProperty(Memory* dram, std::list<uint64_t>* process, Utility::Graph<v_t, e_t>* graph, uint64_t base_addr, std::string name, uint64_t id);
  ~ReadSrcProperty();

  void tick(void);
  void reset(void);
};

} // namespace SimObj

#include "readSrcProperty.tcc"

#endif
