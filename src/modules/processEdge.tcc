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
SimObj::ProcessEdge<v_t, e_t>::ProcessEdge() {
  _state = OP_WAIT;
  _stall = STALL_CAN_ACCEPT;
  _ready = false;
  _counter = 0;
  _delay_cycles = 1;
}

template<class v_t, class e_t>
SimObj::ProcessEdge<v_t, e_t>::ProcessEdge(int delay_cycles, GraphMat::GraphApp<v_t, e_t>* graph_app) {
  assert(graph_app != NULL);
  _graph_app = graph_app;
  _state = OP_WAIT;
  _stall = STALL_CAN_ACCEPT;
  _ready = false;
  _counter = 0;
  _delay_cycles = delay_cycles;
}

template<class v_t, class e_t>
SimObj::ProcessEdge<v_t, e_t>::~ProcessEdge() {
  _graph_app = NULL;
}

template<class v_t, class e_t>
void SimObj::ProcessEdge<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
        // Upstream sent vertex & vertex property
#ifdef MODULE_DEBUG
        std::cout << "Tick:" << _tick << " " << _name << " recieved: " << _data << "\n";
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
  _state = next_state;
  this->update_stats();
}

