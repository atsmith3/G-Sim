/*
 * Andrew Smith
 *
 * Apply
 *  User Defined Function to Apply a Property to a vertex.
 *  min(in the case of SSSP)
 *
 */

#ifndef APPLY_H
#define APPLY_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>

#include "module.h"
#include "memory.h"

class Apply : public Module {
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

public:
  Apply();
  Apply(int delay_cycles);
  ~Apply();

  void tick(void);
  void ready(void);
};

#endif
