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
SimObj::ReadVertexProperty<v_t, e_t>::ReadVertexProperty(Memory* dram, std::list<uint64_t>* apply, Utility::readGraph<v_t>* graph) {
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
  _dram = NULL;
  _apply = NULL;
  _graph = NULL;
}


template<class v_t, class e_t>
void SimObj::ReadVertexProperty<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready && !_apply->empty()) {
        // Dequeue from the apply work queue
        _data.vertex_id = _apply->front();
        _data.vertex_id_addr = _graph->getVertexAddress(_data.vertex_id_addr);
        _apply->pop_front();
        _data.last_edge = false;
        _data.last_vertex = false;
        _items_processed++;

        if(_apply->empty()) {
          _data.last_vertex = true;
          _ready = false;
        }

        // Read the global vertex property
        _data.vertex_data = _graph->getVertexProperty(_data.vertex_id);
        _mem_flag = false;
        _dram->read(_data.vertex_id_addr, &_mem_flag);
        _stall = STALL_MEM;
        next_state = OP_MEM_WAIT;
      }
      else {
        // Wait for upstream to send vertex
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
        _has_work = false;
      }
      break;
    }
    case OP_MEM_WAIT : {
      if(_mem_flag) {
        if(_next->is_stalled() == STALL_CAN_ACCEPT) {
          _next->ready(_data);
          next_state = OP_WAIT;
          _stall = STALL_CAN_ACCEPT;
          _has_work = false;
        }
        else {
          next_state = OP_MEM_WAIT;
          _stall = STALL_PIPE;
        }
      }
      else {
        next_state = OP_MEM_WAIT;
        _stall = STALL_MEM;
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

