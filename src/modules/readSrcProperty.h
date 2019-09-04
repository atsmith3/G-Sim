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

#include "module.h"
#include "memory.h"

namespace SimObj {


class ReadSrcProperty : public Module {
private:
  enum read_src_op_t {
    OP_WAIT,
    OP_FETCH,
    OP_MEM_WAIT,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_FETCH"},
    {2, "OP_MEM_WAIT"},
    {3, "OP_NUM_OPS"}};
#endif

  Memory* _dram;
  read_src_op_t _state;
  bool _fetched;

public:
  bool _mem_flag;
  ReadSrcProperty();
  ReadSrcProperty(Memory* dram);
  ~ReadSrcProperty();

  void tick(void);
  void ready(void);
};

} // namespace SimObj

#endif
