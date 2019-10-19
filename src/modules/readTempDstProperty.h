/*
 * Andrew Smith
 *
 * Read DST Property:
 *  Read the destination property from DRAM. Optional depending on the 
 *  graph algorithm
 *
 */

#ifndef READTEMPDSTPROPERTY_H
#define READTEMPDSTPROPERTY_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>

#include "module.h"
#include "memory.h"

namespace SimObj {

template<class v_t, class e_t>
class ReadTempDstProperty : public Module<v_t, e_t> {
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

  Memory* _scratchpad;
  op_t _state;
  Utility::readGraph<v_t>* _graph;
  std::map<uint64_t, Utility::pipeline_data<v_t, e_t>>* _scratch_mem;

public:
  bool _mem_flag;
  ReadTempDstProperty();
  ReadTempDstProperty(Memory* scratchpad, Utility::readGraph<v_t>* graph, std::map<uint64_t, Utility::pipeline_data<v_t, e_t>>* scratch_mem);
  ~ReadTempDstProperty();

  void tick(void);
};

} // namespace SimObj

#include "readTempDstProperty.tcc"

#endif
