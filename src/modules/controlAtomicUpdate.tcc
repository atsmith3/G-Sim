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
  ready_prev = false;
  dependency_prev = false;
  send_prev = false;
  ready_curr = false;
  dependency_curr = false;
  send_curr = false;
  _in_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_in.csv");
  _out_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_out.csv");
  assert(_in_logger != NULL);
  assert(_out_logger != NULL);
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
#ifdef MODULE_TRACE
  ready_curr = _ready;
  send_curr = _next->is_stalled() == STALL_CAN_ACCEPT;
  dependency_curr = dependency();
#endif

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
#ifdef MODULE_TRACE
  update_logger();
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

#ifdef MODULE_TRACE
template<class v_t, class e_t>
void SimObj::ControlAtomicUpdate<v_t, e_t>::update_logger(void) {
  if(ready_prev != ready_curr ||
     dependency_prev != dependency_curr ||
     send_prev != send_curr) {
    if(_in_logger != NULL) {
      _in_logger->write(std::to_string(_tick)+","+
                     std::to_string(_in_data.vertex_id_addr)+","+
                     std::to_string(_in_data.vertex_dst_id)+","+
                     std::to_string(_in_data.vertex_dst_id_addr)+","+
                     std::to_string(_in_data.edge_id)+","+
                     std::to_string(_in_data.vertex_data)+","+
                     std::to_string(_in_data.vertex_dst_data)+","+
                     std::to_string(_in_data.message_data)+","+
                     std::to_string(_in_data.vertex_temp_dst_data)+","+
                     std::to_string(_in_data.edge_data)+","+
                     std::to_string(_in_data.edge_temp_data)+","+
                     std::to_string(ready_curr)+","+
                     std::to_string(dependency_curr)+","+
                     std::to_string(send_curr)+","+"\n");
    }
    ready_prev = ready_curr;
    dependency_prev = dependency_curr;
    send_prev = send_curr;
  }
}
#endif
