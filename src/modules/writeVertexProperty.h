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

#include "module.h"
#include "memory.h"

namespace SimObj {

class WriteVertexProperty : public Module {
private:
  enum op_t {
    OP_WAIT,
    OP_WRITE,
    OP_MEM_WAIT,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_WRITE"},
    {2, "OP_MEM_WAIT"},
    {3, "OP_NUM_OPS"}};
#endif

  Memory* _dram;
  op_t _state;
  bool _ready;

public:
  bool _mem_flag;
  WriteVertexProperty();
  WriteVertexProperty(Memory* dram);
  ~WriteVertexProperty();

  void tick(void);
  void ready(void);
};

} // namespace SimObj

#endif

