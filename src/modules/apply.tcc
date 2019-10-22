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
SimObj::Apply<v_t, e_t>::Apply() {
  _app = NULL;
  _state = OP_WAIT;
  _stall = STALL_CAN_ACCEPT;
  _ready = false;
  _counter = 0;
  _delay_cycles = 1;
}

template<class v_t, class e_t>
SimObj::Apply<v_t, e_t>::Apply(int delay_cycles, GraphMat::GraphApp<v_t, e_t>* app) {
  assert(app != NULL);
  _app = app;
  _state = OP_WAIT;
  _stall = STALL_CAN_ACCEPT;
  _ready = false;
  _counter = 0;
  _delay_cycles = delay_cycles;
}

template<class v_t, class e_t>
SimObj::Apply<v_t, e_t>::~Apply() {
  _app = NULL;
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
          // Do Apply
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
    std::cout << "[ " << __PRETTY_FUNCTION__ << " ] tick: " << _tick << "  state: " << _state_name[_state] << "  next_state: " << _state_name[next_state] << "\n";
  }
#endif
  _state = next_state;
  this->update_stats();
}
