/*
 * Andrew Smith
 *
 * Read SRC Property:
 *  This is the frontend module of the graphicionado accelerator;
 *  it performs a sequential read on the value of the src vertex.
 *
 *  It has to interface with the DRAM memory to fetch the active
 *  property. Edges are stored in the Scratchpad.
 *
 */

#include <cassert>

template<class v_t, class e_t>
SimObj::ReadSrcProperty<v_t, e_t>::ReadSrcProperty() {
  _dram = NULL;
  _process = NULL;
  _graph = NULL;
  _state = OP_WAIT;
  _mem_flag = false;
  _fetched = false;
}


template<class v_t, class e_t>
SimObj::ReadSrcProperty<v_t, e_t>::ReadSrcProperty(Memory* dram, std::queue<uint64_t>* process, Utility::readGraph<v_t>* graph) {
  assert(dram != NULL);
  assert(process != NULL);
  assert(graph != NULL);
  _graph = graph;
  _dram = dram;
  _process = process;
  _state = OP_WAIT;
  _mem_flag = false;
  _fetched = false;
}


template<class v_t, class e_t>
SimObj::ReadSrcProperty<v_t, e_t>::~ReadSrcProperty() {
  _dram = NULL;
  _process = NULL;
  _graph = NULL;
}


template<class v_t, class e_t>
void SimObj::ReadSrcProperty<v_t, e_t>::tick() {
  this->_tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_fetched && _next->is_stalled() == STALL_CAN_ACCEPT) {
        _next->ready(_data);
        _fetched = false;
        next_state = OP_WAIT;
        _stall = STALL_PROCESSING;
      }
      else if(!_fetched && !_process->empty()) {
        // Dequeue from the work queue
        _data.vertex_id = _process->front();
        _process->pop();
        _data.vertex_data = _graph->getVertexProperty(_data.vertex_id);
        _data.edge_id = 0;

        if(_process->empty()) {
          _data.last_vertex = true;
        }
        else {
          _data.last_vertex = false;
        }

        _mem_flag = false;
        _dram->read(0x100, &_mem_flag);
        _stall = STALL_MEM;
        next_state = OP_MEM_WAIT;
      }
      else {
        next_state = OP_WAIT;
        _stall = STALL_PIPE;
      }
      break;
    }
    case OP_MEM_WAIT : {
      if(_mem_flag) {

        if(_next->is_stalled() == STALL_CAN_ACCEPT) {
          _next->ready(_data);
          _fetched = false;
          _mem_flag = false;
          _dram->read(0x100, &_mem_flag);
          _stall = STALL_MEM;
          next_state = OP_MEM_WAIT;
        }
        else {
          next_state = OP_WAIT;
          _stall = STALL_PIPE;
          _fetched = true;
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
#ifdef DEBUG
  if(_state != next_state) {
    std::cout << "[ " << __PRETTY_FUNCTION__ << " ] tick: " << this->_tick << "  state: " << _state_name[_state] << "  next_state: " << _state_name[next_state] << "\n";
  }
#endif
  _state = next_state;
  this->update_stats();
}
