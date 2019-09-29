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
  _vertex_id = 0;
  _edge_id = 0;
}


SimObj::ControlAtomicUpdate::~ControlAtomicUpdate() {
  // Do nothing
}

bool SimObj::ControlAtomicUpdate::dependency() {
  bool ret = false;
  for(auto it = _edges.begin(); it != _edges.end(); it++) {
    if(_edge_id == *it) {
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
        if(!dependency() && _next->is_stalled() == STALL_CAN_ACCEPT) {
          _next->ready(_vertex_id, _edge_id);
          _edges.push_front(_edge_id);
          next_state = OP_WAIT;
          _stall = STALL_CAN_ACCEPT;
        }
        else {
          next_state = OP_STALL;
          _stall = STALL_PROCESSING;
        }
      }
      else {
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
      }
      break;
    }
    // If there is a dep stall the pipe until the edge completes
    case OP_STALL : {
      // Check if an edge was finalized:
      if(!dependency() && _next->is_stalled() == STALL_CAN_ACCEPT) {
        _next->ready(_vertex_id, _edge_id);
        _edges.push_front(_edge_id);
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
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

void SimObj::ControlAtomicUpdate::receive_message(msg_t msg) {
  if(msg == MSG_ATOMIC_OP_COMPLETE) {
    _edges.pop_back();
  }
}
