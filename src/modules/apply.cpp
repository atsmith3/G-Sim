/*
 * Andrew Smith
 *
 * Apply
 *  User Defined Function to Apply a Property to a vertex.
 *  min(in the case of SSSP)
 *
 */


#include <cassert>

#include "apply.h"


SimObj::Apply::Apply() {
  _state = OP_WAIT;
  _stall = STALL_CAN_ACCEPT;
  _ready = false;
  _counter = 0;
  _delay_cycles = 1;
}

SimObj::Apply::Apply(int delay_cycles) {
  _state = OP_WAIT;
  _stall = STALL_CAN_ACCEPT;
  _ready = false;
  _counter = 0;
  _delay_cycles = delay_cycles;
}

SimObj::Apply::~Apply() {
  // Do Nothing
}

void SimObj::Apply::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
        // Upstream sent vertex & vertex property
        _ready = false;
        _counter = 0;
        next_state = OP_COUNT;
        _stall = STALL_PROCESSING;
      }
      else {
        // Wait for upstream to send vertex
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
      }
      break;
    }
    case OP_COUNT : {
      if(_counter < _delay_cycles) {
        next_state = OP_COUNT;
        _stall = STALL_PROCESSING;
      }
      else {
        _next->ready();
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
      }
      _counter++;
      break;
    }
    default : {

    }
  }
#ifdef DEBUG
  if(_state != next_state) {
    std::cout << "[ " << __PRETTY_FUNCTION__ << " ] tick: " << _tick << "  state: " << _state_name[_state] << "  next_state: " << _state_name[next_state] << "\n";
  }
#endif
  _state = next_state;
  update_stats();

}

void SimObj::Apply::ready(void) {
  _ready = true;
}
