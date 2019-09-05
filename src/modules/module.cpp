/*
 * Andrew Smith
 *
 * Base Class for a Pipeline Module
 *
 */

#include "module.h"

SimObj::Module::Module() {
  _tick = 0;
  _stall = STALL_CAN_ACCEPT;
  _stall_count = 0;
  _stall_ticks.resize(STALL_NUM_TYPES, 0);
  _next = NULL;
  _prev = NULL;
}


SimObj::Module::~Module() {
  // Do Nothing
}

SimObj::stall_t SimObj::Module::is_stalled(void) {
  return _stall;
}

void SimObj::Module::tick(void) {
  _tick++;
}

void SimObj::Module::recieve_message(SimObj::msg_t msg) {
  // Process Message
}

uint64_t SimObj::Module::get_attr(void) {
  // TODO: Return Vertex ID
  return 0;
}

void SimObj::Module::ready(void) {
  // Does Nothing
}

void SimObj::Module::set_next(SimObj::Module* next) {
  _next = next;
}
  
void SimObj::Module::set_prev(SimObj::Module* prev) {
  _prev = prev;
}

void SimObj::Module::update_stats() {
  _stall_ticks[_stall]++;
  if(_stall != STALL_CAN_ACCEPT) {
    _stall_count++;
  }
}

#ifdef DEBUG
void SimObj::Module::set_stall(stall_t stall) {
  _stall = stall;
}
#endif
