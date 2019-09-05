/*
 * Andrew Smith
 *
 * Read SRC Edges:
 *  The Read SRC Edges Module is the second module in the pipeline.
 *  It reads the list of edges of the current vertex from Scratchpad Memory.
 *
 */

#ifndef READSRCEDGES_H
#define READSRCEDGES_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>

#include "module.h"
#include "memory.h"

namespace SimObj {


class ReadSrcEdges : public Module {
private:
  enum op_t {
    OP_WAIT,
    OP_FETCH,
    OP_MEM_WAIT,
    OP_SEND_EDGE_DOWNSTREAM,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_FETCH"},
    {2, "OP_MEM_WAIT"},
    {3, "OP_SEND_EDGE_DOWNSTREAM"},
    {4, "OP_NUM_OPS"}};
#endif

  Memory* _scratchpad;
  op_t _state;
  bool _ready;
  std::vector<int> _edge_list;
  std::vector<int>::iterator _edge_it;

public:
  bool _mem_flag;
  ReadSrcEdges();
  ReadSrcEdges(Memory* dram);
  ~ReadSrcEdges();

  void tick(void);
  void ready(void);
};

} // namespace SimObj

#endif
