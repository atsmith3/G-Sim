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
  using Module<v_t, e_t>::_stall_ticks;
  using Module<v_t, e_t>::_has_work;

  Memory* _dram;
  op_t _state;
  uint64_t _throughput;
  bool _complete;

  Utility::readGraph<v_t>* _graph;
  std::list<uint64_t>* _process;

public:
  bool _mem_flag;
  WriteVertexProperty();
  WriteVertexProperty(Memory* dram, std::list<uint64_t>* process, Utility::readGraph<v_t>* graph);
  ~WriteVertexProperty();

  void tick(void);

  bool complete();
  void flush();

  void print_stats(void);
  void print_stats_csv(void);
};

} // namespace SimObj

#include "writeVertexProperty.tcc"

#endif

