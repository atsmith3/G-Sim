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

namespace SimObj {


class ReadTempTempVertexProperty : public Module {
private:
  enum op_t {
    OP_WAIT,
    OP_FETCH,
    OP_MEM_WAIT,
    OP_SEND_DOWNSTREAM,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_FETCH"},
    {2, "OP_MEM_WAIT"},
    {3, "OP_SEND_DOWNSTREAM"},
    {4, "OP_NUM_OPS"}};
#endif

  Memory* _dram;
  op_t _state;
  bool _ready;

public:
  bool _mem_flag;
  ReadTempVertexProperty();
  ReadTempVertexProperty(Memory* dram);
  ~ReadTempVertexProperty();

  void tick(void);
  void ready(void);
};

} // namespace SimObj

#endif
