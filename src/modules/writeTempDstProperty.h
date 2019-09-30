/*
 * Andrew Smith
 *
 * WriteTempDstProperty
 *  This is the last stage in the processing phase, it writes the temp value
 *  back to memory and tells the ControlAtomicUpdate module a edge has finished.
 */

#ifndef WRITETEMPDSTPROPERTY_H
#define WRITETEMPDSTPROPERTY_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>

#include "module.h"
#include "memory.h"
#include "controlAtomicUpdate.h"

#include "readGraph.h"

namespace SimObj {

template<class v_t, class e_t>
class WriteTempDstProperty : public Module<v_t, e_t> {
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

  Memory* _scratchpad;
  op_t _state;
  bool _ready;
  ControlAtomicUpdate<v_t, e_t>* _cau;

  std::map<uint64_t, Utility::pipeline_data<v_t, e_t>>* _scratch_mem;

  uint64_t _edges_written;

public:
  bool _mem_flag;
  WriteTempDstProperty();
  WriteTempDstProperty(Memory* scratchpad, ControlAtomicUpdate* cau, std::map<uint64_t, Utility::pipeline_data<v_t, e_t>>* scratch_mem);
  ~WriteTempDstProperty();

  void tick(void);
  void ready(void);
  void print_stats(void);
  void print_stats_csv(void);
};

} // namespace SimObj

#include "writeTempDstProperty.tcc"

#endif // WRITETMPDSTPROPERTY_H
