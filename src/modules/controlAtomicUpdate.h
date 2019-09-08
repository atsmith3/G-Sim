/*
 * Andrew Smith
 *
 * Control Atomic Update:
 *  The control atomic update module is part of the atomic update sequence of the
 *  graphicionado pipeline.
 *
 */

#ifndef CONTROLATOMICUPDATE_H
#define CONTROLATOMICUPDATE_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>
#include <list>

#include "module.h"
#include "memory.h"

namespace SimObj {

class ControlAtomicUpdate : public Module {
private:
  enum op_t {
    OP_WAIT,
    OP_CHECK_DEP,
    OP_STALL,
    OP_SEND,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_CHECK_DEP"},
    {2, "OP_STALL"},
    {3, "OP_SEND"},
    {4, "OP_NUM_OPS"}};
#endif

  op_t _state;
  bool _ready;
  bool _op_complete;
  std::list<uint64_t> _edges;
  uint64_t _cur_edge;

  bool dependency();

public:
  ControlAtomicUpdate();
  ~ControlAtomicUpdate();

  void tick(void);
  void ready(void);
  void receive_message(msg_t msg);
};

} // namespace SimObj


#endif
