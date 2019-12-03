/*
 * Andrew Smith
 *
 * Apply
 *  User Defined Function to Apply a Property to a vertex.
 *  min(in the case of SSSP)
 *
 */


#include <cassert>

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
  _logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+".csv");
  assert(_logger != NULL);
#endif
}

template<class v_t, class e_t>
SimObj::Apply<v_t, e_t>::~Apply() {
  _app = NULL;
#if MODULE_TRACE
  if(_logger) {
    delete _logger;
    _logger = NULL;
  }
#endif
}

template<class v_t, class e_t>
void SimObj::Apply<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
        // Upstream sent vertex & vertex property
#ifdef MODULE_TRACE
        // Ready, Complete, Sent
        _logger->write(std::to_string(_tick)+",1,0,0\n");
        complete_logged = false;
#endif
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
        if(_next->is_stalled() == STALL_CAN_ACCEPT) {
#ifdef MODULE_TRACE
          _logger->write(std::to_string(_tick)+",0,0,1\n");
#endif
          // Do Apply
          _data.updated = _app->apply(_data.vertex_temp_dst_data, _data.vertex_data);
          _next->ready(_data);
          next_state = OP_WAIT;
          _stall = STALL_CAN_ACCEPT;
          _has_work = false;
        }
        else {
#ifdef MODULE_TRACE
          if(!complete_logged) {
            _logger->write(std::to_string(_tick)+",0,1,0\n");
            complete_logged = true;
          }
#endif
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
  _state = next_state;
  this->update_stats();
}
