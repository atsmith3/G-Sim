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
  uint64_t _avg_connectivity;
  std::vector<int> _edge_list;
  std::vector<int>::iterator _edge_it;

public:
  bool _mem_flag;
  ReadSrcEdges();
  ReadSrcEdges(Memory* dram);
  ReadSrcEdges(Memory* dram, uint64_t avg_connectivity);
  ~ReadSrcEdges();

  void tick(void);
  void ready(void);
};

} // namespace SimObj

#endif
