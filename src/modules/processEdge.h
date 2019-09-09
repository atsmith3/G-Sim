/*
 * Andrew Smith
 *
 * User Definable Function to process the edge.
 *  Used in certain applications.
 *  Purely computational delay
 *
 */

#ifndef PROCESSEDGE_H
#define PROCESSEDGE_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>

#include "module.h"
#include "memory.h"

namespace SimObj {

class ProcessEdge : public Module {
private:
  enum op_t {
    OP_WAIT,
    OP_COUNT,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_COUNT"},
    {2, "OP_NUM_OPS"}};
#endif

  op_t _state;
  uint64_t _counter;
  bool _ready;
  uint64_t _delay_cycles;

public:
  ProcessEdge();
  ProcessEdge(int delay_cycles);
  ~ProcessEdge();

  void tick(void);
  void ready(void);
};

} // namespace SimObj

#endif
