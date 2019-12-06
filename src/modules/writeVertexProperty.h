/*
 *
 * Andrew Smith
 * 
 * WriteVertexProperty
 *  This is the last stage in the apply phase
 *  Writes the vertex value back to memory
 */

#ifndef WRITEVERTEXPROPERTY_H
#define WRITEVERTEXPROPERTY_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>
#include <list>

#include "module.h"
#include "memory.h"
#include "log.h"

#include "readGraph.h"

namespace SimObj {

template<class v_t, class e_t>
class WriteVertexProperty : public Module<v_t, e_t> {
private:
  enum op_t {
    OP_WAIT,
    OP_MEM_WAIT,
    OP_NUM_OPS
  };

  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_MEM_WAIT"},
    {2, "OP_NUM_OPS"}};

  using Module<v_t, e_t>::_tick;
  using Module<v_t, e_t>::_ready;
  using Module<v_t, e_t>::_stall;
  using Module<v_t, e_t>::_next;
  using Module<v_t, e_t>::_data;
  using Module<v_t, e_t>::_name;
  using Module<v_t, e_t>::_stall_ticks;
  using Module<v_t, e_t>::_has_work;
#if MODULE_TRACE
  using Module<v_t, e_t>::_in_data;
  using Module<v_t, e_t>::_in_logger;
  using Module<v_t, e_t>::_out_logger;
  bool ready_prev;
  bool mem_flag_prev;
  uint64_t address_prev;
  uint64_t mem_out_prev;
  bool ready_curr;
  bool mem_flag_curr;
  uint64_t address_curr;
  uint64_t mem_out_curr;

  void update_logger();
#endif

  Memory* _dram;
  op_t _state;
  uint64_t _throughput;

  uint64_t _base_addr;
  uint64_t _curr_addr;

  uint64_t _id;

  Utility::readGraph<v_t>* _graph;
  std::list<uint64_t>* _process;

public:
  bool _mem_flag;
  WriteVertexProperty(Memory* dram, std::list<uint64_t>* process, Utility::readGraph<v_t>* graph, uint64_t base_addr, std::string name, uint64_t id);
  ~WriteVertexProperty();

  void tick(void);
  void reset(void);

  void print_stats(void);
  void print_stats_csv(void);
};

} // namespace SimObj

#include "writeVertexProperty.tcc"

#endif

