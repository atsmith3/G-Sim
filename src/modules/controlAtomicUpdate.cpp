/*
 * Andrew Smith
 *
 * Control Atomic Update:
 *  The control atomic update module is part of the atomic update sequence of the
 *  graphicionado pipeline.
 *
 */

#include <cassert>

#include "controlAtomicUpdate.h"


SimObj::ControlAtomicUpdate::ControlAtomicUpdate() {
  _state = OP_WAIT;
  _ready = false;
  _op_complete = false;
}


SimObj::ControlAtomicUpdate::~ControlAtomicUpdate() {
  // Do nothing
}

bool SimObj::ControlAtomicUpdate::dependency() {
  bool ret = false;
  for(auto it = _edges.begin(); it != _edges.end(); it++) {
    if(_cur_edge == *it) {
      ret = true;
      break;
    }
  }
  return ret;
}

void SimObj::ControlAtomicUpdate::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
        _ready = false;
        next_state = OP_CHECK_DEP;
        _stall = STALL_PROCESSING;
      }
      else {
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
      }
      break;
    }
    // Check if the same edge is in the pipe
    case OP_CHECK_DEP : {
      if(dependency()) {
        _stall = STALL_PIPE;
        next_state = OP_STALL;
      }
      else {
        _stall = STALL_PROCESSING;
        next_state = OP_SEND;
      }
      break;
    }
    case OP_SEND : {
      if(_next->is_stalled() == STALL_CAN_ACCEPT) {
        _next->ready();
        _edges.push_front(_cur_edge);
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
      }
      else {
        next_state = OP_SEND;
        _stall = STALL_PROCESSING;
      }
      break;
    }
    // If there is a dep stall the pipe until the edge completes
    case OP_STALL : {
      // Check if an edge was finalized:
      if(!dependency()) {
        next_state = OP_SEND;
        _stall = STALL_PIPE;
      }
      else {
        next_state = OP_STALL;
        _stall = STALL_PIPE;
      }
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


void SimObj::ControlAtomicUpdate::ready(void) {
  _ready = true;
  _cur_edge = rand() % 400;
}


void SimObj::ControlAtomicUpdate::receive_message(msg_t msg) {
  if(msg == MSG_ATOMIC_OP_COMPLETE) {
    _edges.pop_back();
  }
}
