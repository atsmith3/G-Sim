/*
 * Andrew Smith
 *
 * Read Temp TempVertex Property:
 *  Read the Temp TempVertex property from the scratchpad.
 *
 */

#ifndef READTEMPVERTEXPROPERTY_H
#define READTEMPVERTEXPROPERTY_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>

#include "module.h"
#include "memory.h"

#include "readGraph.h"

namespace SimObj {

template<class v_t, class e_t>
class ReadTempVertexProperty : public Module<v_t, e_t> {
private:
  enum op_t {
    OP_WAIT,
    OP_MEM_WAIT,
    OP_SEND_DOWNSTREAM,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_MEM_WAIT"},
    {2, "OP_SEND_DOWNSTREAM"},
    {3, "OP_NUM_OPS"}};
#endif

  using Module<v_t, e_t>::_tick;
  using Module<v_t, e_t>::_ready;
  using Module<v_t, e_t>::_data;
  using Module<v_t, e_t>::_stall;
  using Module<v_t, e_t>::_next;

  Memory* _dram;
  op_t _state;
  Utility::readGraph<v_t>* _graph;
  std::map<uint64_t, Utility::pipeline_data<v_t, e_t>>* _scratch_mem;

public:
  bool _mem_flag;
  ReadTempVertexProperty();
  ReadTempVertexProperty(Memory* dram, Utility::readGraph<v_t>* graph, std::map<uint64_t, Utility::pipeline_data<v_t, e_t>>* scratch_mem);
  ~ReadTempVertexProperty();

  void tick(void);
};

} // namespace SimObj

#include "readTempVertexProperty.tcc"

#endif
