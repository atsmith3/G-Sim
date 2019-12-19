/*
 * Andrew Smith
 *
 * Apply
 *  User Defined Function to Apply a Property to a vertex.
 *  min(in the case of SSSP)
 *
 */


#include <cassert>
#include <sstream>

template<class v_t, class e_t>
SimObj::Apply<v_t, e_t>::Apply(int delay_cycles, GraphMat::GraphApp<v_t, e_t>* app, std::string name, uint64_t id) {
  assert(app != NULL);
  _id = id;
  _name = name;
  _app = app;
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
SimObj::Apply<v_t, e_t>::~Apply() {
  _app = NULL;
#if MODULE_TRACE
  if(_in_logger) {
    delete _in_logger;
    _in_logger = NULL;
  }
  if(_out_logger) {
    delete _out_logger;
    _out_logger = NULL;
  }
#endif
}

template<class v_t, class e_t>
void SimObj::Apply<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state = _state;
#ifdef MODULE_TRACE
  ready_curr = _ready;
  send_curr = _next->is_stalled() == STALL_CAN_ACCEPT;
#endif

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
        _has_work = false;
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
          _data.updated = _app->apply(_data.vertex_temp_dst_data, _data.vertex_data);
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
    sim_out.write("[ " + (std::string)__PRETTY_FUNCTION__ + " ] tick: " + std::to_string(_tick) + "  state: " + _state_name[_state] + "  next_state: " + _state_name[next_state] + "\n");
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
void SimObj::Apply<v_t, e_t>::update_logger(void) {
  std::stringstream out;
  if(ready_prev != ready_curr ||
     complete_prev != complete_curr ||
     send_prev != send_curr) {
    if(_in_logger != NULL) {
      out << _tick << ",";
      out << _in_data << ",";
      out << ready_curr << ",";
      out << complete_curr << ",";
      out << send_curr << "\n";
      _in_logger->write(out.str());
      //_in_logger->write(std::to_string(_tick)+","+
      //               std::to_string(_in_data.vertex_id)+","+
      //               std::to_string(_in_data.vertex_id_addr)+","+
      //               std::to_string(_in_data.vertex_dst_id)+","+
      //               std::to_string(_in_data.vertex_dst_id_addr)+","+
      //               std::to_string(_in_data.edge_id)+","+
      //               std::to_string(_in_data.vertex_data)+","+
      //               std::to_string(_in_data.vertex_dst_data)+","+
      //               std::to_string(_in_data.message_data)+","+
      //               std::to_string(_in_data.vertex_temp_dst_data)+","+
      //               std::to_string(_in_data.edge_data)+","+
      //               std::to_string(_in_data.edge_temp_data)+","+
      //               std::to_string(_in_data.last_vertex)+","+
      //               std::to_string(_in_data.last_edge)+","+
      //               std::to_string(_in_data.updated)+","+
      //               std::to_string(ready_curr)+","+
      //               std::to_string(complete_curr)+","+
      //               std::to_string(send_curr)+"\n");
    }
    ready_prev = ready_curr;
    complete_prev = complete_curr;
    send_prev = send_curr;
  }
}
#endif
