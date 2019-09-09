/*
 * Andrew Smith
 *
 * Reduction Function
 * 
 * User Programmable to perform graph application task.
 *  Hardcoded computational delay for now,
 *  ToDo: Actually perform graph operation.
 *
 */


#ifndef READDSTPROPERTY_H
#define READDSTPROPERTY_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>

#include "module.h"
#include "memory.h"

namespace SimObj {


class Reduce : public Module {
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
    {4, "OP_NUM_OPS"}};
#endif

  op_t _state;
  uint64_t _counter;
  bool _ready;
  uint64_t _delay_cycles;

public:
  Reduce();
  Reduce(int delay_cycles);
  ~Reduce();

  void tick(void);
  void ready(void);
};

} // namespace SimObj

#endif
