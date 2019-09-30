/*
 * Andrew Smith
 *
 * Read Vertex Property:
 *  Read the Vertex property from DRAM.
 */

#include <cassert>

template<class v_t, class e_t>
SimObj::ReadVertexProperty<v_t, e_t>::ReadVertexProperty() {
  _dram = NULL;
  _apply = NULL;
  _graph = NULL;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
}


template<class v_t, class e_t>
SimObj::ReadVertexProperty<v_t, e_t>::ReadVertexProperty(Memory* dram, std::queue<uint64_t>* apply, Utility::readGraph<v_t>* graph) {
  assert(dram != NULL);
  assert(apply != NULL);
  assert(graph != NULL);
  _apply = apply;
  _graph = graph;
  _dram = dram;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
}


template<class v_t, class e_t>
SimObj::ReadVertexProperty<v_t, e_t>::~ReadVertexProperty() {
  // Do Nothing
}


template<class v_t, class e_t>
void SimObj::ReadVertexProperty<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(true) {
        // Upstream sent vertex & vertex property
        _ready = false;
        _mem_flag = false;
        _dram->read(0x01, &_mem_flag);
        _stall = STALL_MEM;
        next_state = OP_MEM_WAIT;
      }
      else {
        // Wait for upstream to send vertex
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
      }
      break;
    }
    case OP_MEM_WAIT : {
      if(_mem_flag) {
        // Dequeue from the apply work queue
        _data.vertex_id = apply->front();
        apply->pop();

        // Read the global vertex property
        _data.vertex_data = _graph->getVertexProperty(_data.vertex_id);

        if(_next->is_stalled() == STALL_CAN_ACCEPT) {
          _next->ready(_data);
          next_state = OP_WAIT;
          _stall = STALL_CAN_ACCEPT;
        }
        else {
          next_state = OP_SEND_DOWNSTREAM;
          _stall = STALL_PROCESSING;
        }
      }
      else {
        next_state = OP_MEM_WAIT;
        _stall = STALL_MEM;
      }
      break;
    }
    case OP_SEND_DOWNSTREAM : {
      if(_next->is_stalled() == STALL_CAN_ACCEPT) {
        _next->ready(_data);
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
      }
      else {
        next_state = OP_SEND_DOWNSTREAM;
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

