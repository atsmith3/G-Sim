/*
 * Andrew Smith
 *
 * Control Atomic Update:
 *  The control atomic update module is part of the atomic update sequence of the
 *  graphicionado pipeline.
 *
 */

#include <cassert>

template<class v_t, class e_t>
SimObj::ControlAtomicUpdate<v_t, e_t>::ControlAtomicUpdate() {
  _state = OP_WAIT;
  _ready = false;
  _op_complete = false;
}

template<class v_t, class e_t>
SimObj::ControlAtomicUpdate<v_t, e_t>::~ControlAtomicUpdate() {
  // Do nothing
}

template<class v_t, class e_t>
bool SimObj::ControlAtomicUpdate<v_t, e_t>::dependency() {
  bool ret = false;
  for(auto it = _nodes.begin(); it != _nodes.end(); it++) {
    if(_data.edge_id == it->edge_id) {
      ret = true;
      break;
    }
  }
  return ret;
}

template<class v_t, class e_t>
void SimObj::ControlAtomicUpdate<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
        _ready = false;
        if(!dependency() && _next->is_stalled() == STALL_CAN_ACCEPT) {
          _next->ready(_data);
          _nodes.push_front(_data);
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
        _next->ready(_data);
        _nodes.push_front(_data);
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
#if 0
  if(_state != next_state) {
    std::cout << "[ " << __PRETTY_FUNCTION__ << " ] tick: " << _tick << "  state: " << _state_name[_state] << "  next_state: " << _state_name[next_state] << "\n";
  }
#endif
  _state = next_state;
  this->update_stats();
}

template<class v_t, class e_t>
void SimObj::ControlAtomicUpdate<v_t, e_t>::receive_message(msg_t msg) {
  if(msg == MSG_ATOMIC_OP_COMPLETE) {
    _nodes.pop_back();
  }
}
