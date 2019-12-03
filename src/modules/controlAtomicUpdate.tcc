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
SimObj::ControlAtomicUpdate<v_t, e_t>::ControlAtomicUpdate(std::string name, uint64_t id) {
  _id = id;
  _name = name;
  _state = OP_WAIT;
  _ready = false;
  _op_complete = false;
#if MODULE_TRACE
  _logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+".csv");
  assert(_logger != NULL);
#endif
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
#ifdef MODULE_TRACE
        // Ready, Dependency, Sent
        _logger->write(std::to_string(_tick)+",1,0,0\n");
        dep_logged = false;
#endif
        _ready = false;
        if(!dependency() && _next->is_stalled() == STALL_CAN_ACCEPT) {
          _next->ready(_data);
          _nodes.push_front(_data);
          next_state = OP_WAIT;
          _stall = STALL_CAN_ACCEPT;
          _has_work = false;
        }
        else {
          next_state = OP_STALL;
          _stall = STALL_PIPE;
        }
      }
      else {
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
        _has_work = false;
      }
      break;
    }
    // If there is a dep stall the pipe until the edge completes
    case OP_STALL : {
      // Check if an edge was finalized:
      //std::cout << "Dependency: " << dependency() << " Queue Size: " << _nodes.size() << "\n";
      if(!dependency() && _next->is_stalled() == STALL_CAN_ACCEPT) {
#ifdef MODULE_TRACE
        _logger->write(std::to_string(_tick)+",0,0,1\n");
#endif
        _next->ready(_data);
        _nodes.push_front(_data);
#ifdef DEBUG
          assert(_nodes.size() < 4);
#endif
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
        _has_work = false;
      }
      else {
#ifdef MODULE_TRACE
        if(!dep_logged) {
          _logger->write(std::to_string(_tick)+",0,1,0\n");
          dep_logged = true;
        }
#endif
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
Utility::pipeline_data<v_t, e_t> SimObj::ControlAtomicUpdate<v_t, e_t>::signal(void) {
  Utility::pipeline_data<v_t, e_t> ret = _nodes.back();
  _nodes.pop_back();
  return ret;
}


template<class v_t, class e_t>
void SimObj::ControlAtomicUpdate<v_t, e_t>::debug(void) {
  std::cout << "[";
  for(auto it = _nodes.begin(); it != _nodes.end(); it++) {
    std::cout << *it << ",";
  }
  std::cout << "]\n";
}
