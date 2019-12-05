/*
 * Andrew Smith
 *
 * Reduction Function
 * 
 * User Programmable to perform graph application task.
 *  Hardcoded computational delay for now,
 *  ToDo: Actually perform graph operation.
 *
 */

#include <cassert>

template<class v_t, class e_t>
SimObj::Reduce<v_t, e_t>::Reduce(int delay_cycles, GraphMat::GraphApp<v_t, e_t>* app, std::string name, uint64_t id) {
  assert(app != NULL);
  _name = name;
  _id = id;
  _app = app;
  _state = OP_WAIT;
  _stall = STALL_CAN_ACCEPT;
  _ready = false;
  _counter = 0;
  _delay_cycles = delay_cycles;
#if MODULE_TRACE
  _in_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_in.csv");
  _out_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_out.csv");
  assert(_in_logger != NULL);
  assert(_out_logger != NULL);
#endif
}

template<class v_t, class e_t>
SimObj::Reduce<v_t, e_t>::~Reduce() {
  _app = NULL;
}

template<class v_t, class e_t>
void SimObj::Reduce<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;
#ifdef MODULE_TRACE
  ready_curr = _ready;
  send_curr = _next->is_stalled() == STALL_CAN_ACCEPT;
#endif

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
#ifdef MODULE_TRACE
        complete_curr = false;
#endif
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
        _counter++;
      }
      else {
#ifdef MODULE_TRACE
        complete_curr = true;
#endif
        if(_next->is_stalled() == STALL_CAN_ACCEPT) {
          _app->reduce(_data.vertex_temp_dst_data, _data.message_data);
          _next->ready(_data);
          next_state = OP_WAIT;
          _stall = STALL_CAN_ACCEPT;
          _has_work = false;
        }
        else {
          next_state = OP_COUNT;
          _stall = STALL_PIPE;
        }
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

#ifdef MODULE_TRACE
template<class v_t, class e_t>
void SimObj::Reduce<v_t, e_t>::update_logger(void) {
  if(ready_prev != ready_curr ||
     complete_prev != complete_curr ||
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
                     std::to_string(mem_flag_curr)+","+
                     std::to_string(send_curr)+","+
                     std::to_string(mem_result_curr)+"\n");
    }
    ready_prev = ready_curr;
    complete_prev = complete_curr;
    send_prev = send_curr;
  }
}
#endif
