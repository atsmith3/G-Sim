/*
 * Andrew Smith
 *
 * User Definable Function to process the edge.
 *  Used in certain applications.
 *  Purely computational delay
 *
 */

#include <cassert>

template<class v_t, class e_t>
SimObj::ProcessEdge<v_t, e_t>::ProcessEdge(int delay_cycles, GraphMat::GraphApp<v_t, e_t>* graph_app, std::string name, uint64_t id) {
  assert(graph_app != NULL);
  _id = id;
  _name = name;
  _graph_app = graph_app;
  _state = OP_WAIT;
  _stall = STALL_CAN_ACCEPT;
  _ready = false;
  _counter = 0;
  _delay_cycles = delay_cycles;
#if MODULE_TRACE
  ready_prev = false;
  complete_prev = false;
  send_prev = false;
  ready_curr = false;
  complete_curr = false;
  send_curr = false;
  _in_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_in.csv");
  _out_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_out.csv");
  assert(_in_logger != NULL);
  assert(_out_logger != NULL);
#endif
}

template<class v_t, class e_t>
SimObj::ProcessEdge<v_t, e_t>::~ProcessEdge() {
  _graph_app = NULL;
}

template<class v_t, class e_t>
void SimObj::ProcessEdge<v_t, e_t>::tick(void) {
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
          _graph_app->process_edge(_data.message_data, _data.edge_data, _data.vertex_data);
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
void SimObj::ProcessEdge<v_t, e_t>::update_logger(void) {
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
                     std::to_string(_in_data.last_vertex)+","+
                     std::to_string(_in_data.last_edge)+","+
                     std::to_string(_in_data.updated)+","+
                     std::to_string(ready_curr)+","+
                     std::to_string(complete_curr)+","+
                     std::to_string(send_curr)+","+"\n");
    }
    ready_prev = ready_curr;
    complete_prev = complete_curr;
    send_prev = send_curr;
  }
}
#endif
