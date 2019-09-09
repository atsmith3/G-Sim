/*
 * Andrew Smith
 *
 * Read Vertex Property:
 *  Read the vertex property from DRAM.
 *
 */

#ifndef READVERTEXPROPERTY_H
#define READVERTEXPROPERTY_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>

#include "module.h"
#include "memory.h"

namespace SimObj {


class ReadVertexProperty : public Module {
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
  ReadVertexProperty();
  ReadVertexProperty(Memory* dram);
  ~ReadVertexProperty();

  void tick(void);
  void ready(void);
};

} // namespace SimObj

#endif


