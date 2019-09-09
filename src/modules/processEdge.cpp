/*
 * Andrew Smith
 *
 * User Definable Function to process the edge.
 *  Used in certain applications.
 *  Purely computational delay
 *
 */

#include <cassert>

#include "processEdge.h"


SimObj::ProcessEdge::ProcessEdge() {
  _state = OP_WAIT;
  _stall = STALL_CAN_ACCEPT;
  _ready = false;
  _counter = 0;
  _delay_cycles = 1;
}

SimObj::ProcessEdge::ProcessEdge(int delay_cycles) {
  _state = OP_WAIT;
  _stall = STALL_CAN_ACCEPT;
  _ready = false;
  _counter = 0;
  _delay_cycles = delay_cycles;
}

SimObj::ProcessEdge::~ProcessEdge() {
  // Do Nothing
}

void SimObj::ProcessEdge::tick(void) {
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

void SimObj::ProcessEdge::ready(void) {
  _ready = true;
}
