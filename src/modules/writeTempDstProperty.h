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

namespace SimObj {

class WriteTempDstProperty : public Module {
private:
  enum op_t {
    OP_WAIT,
    OP_WRITE,
    OP_MEM_WAIT,
    OP_SIGNAL_CAU,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_WRITE"},
    {2, "OP_MEM_WAIT"},
    {3, "OP_SIGNAL_CAU"},
    {4, "OP_NUM_OPS"}};
#endif

  Memory* _scratchpad;
  op_t _state;
  bool _ready;
  Module* _cau;

public:
  bool _mem_flag;
  WriteTempDstProperty();
  WriteTempDstProperty(Memory* scratchpad, Module* cau);
  ~WriteTempDstProperty();

  void tick(void);
  void ready(void);
};

} // namespace SimObj

#endif
