/*
 *
 * Andrew Smith
 * 
 * WriteVertexProperty
 *  This is the last stage in the apply phase
 *  Writes the vertex value back to memory
 */

#include <cassert>


template<class v_t, class e_t>
SimObj::WriteVertexProperty<v_t, e_t>::WriteVertexProperty() {
  _dram = NULL;
  _process = NULL;
  _graph = NULL;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
  _throughput = 0;
  _complete = false;
}


template<class v_t, class e_t>
SimObj::WriteVertexProperty<v_t, e_t>::WriteVertexProperty(Memory* dram, std::list<uint64_t>* process, Utility::readGraph<v_t>* graph) {
  assert(dram != NULL);
  assert(graph != NULL);
  assert(process != NULL);
  _graph = graph;
  _process = process;
  _dram = dram;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
  _throughput = 0;
  _complete = false;
}


template<class v_t, class e_t>
SimObj::WriteVertexProperty<v_t, e_t>::~WriteVertexProperty() {
  _dram = NULL;
  _process = NULL;
  _graph = NULL;
}


template<class v_t, class e_t>
void SimObj::WriteVertexProperty<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready && !_complete) {
        // Upstream sent _edge property
        _ready = false;
        _mem_flag = false;
        _dram->write(0x01, &_mem_flag);
        _stall = STALL_MEM;
        next_state = OP_MEM_WAIT;
      }
      else {
        // Wait for upstream to send _edge
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
      }
      break;
    }
    case OP_MEM_WAIT : {
      if(_mem_flag) {
        // Write to global mem
        if(_data.updated) {
          _graph->setVertexProperty(_data.vertex_id, _data.vertex_data);
          _process->push_back(_data.vertex_id);
          _throughput++;
        }
        if(_data.last_vertex) {
          _complete = true;
        }
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
        _has_work = false;
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


template<class v_t, class e_t>
void SimObj::WriteVertexProperty<v_t, e_t>::print_stats(void) {
  std::cout << "-------------------------------------------------------------------------------\n";
  std::cout << "[ " << _name << " ]\n";
  std::cout << "  Stalls:\n";
  std::cout << "    STALL_CAN_ACCEPT: " << _stall_ticks[STALL_CAN_ACCEPT] << " cycles\n";
  std::cout << "    STALL_PROCESSING: " << _stall_ticks[STALL_PROCESSING] << " cycles\n";
  std::cout << "    STALL_PIPE:       " << _stall_ticks[STALL_PIPE] << " cycles\n";
  std::cout << "    STALL_MEM:        " << _stall_ticks[STALL_MEM] << " cycles\n";
  std::cout << "  Performance:\n";
  std::cout << "    Vertex:           " << _throughput << "\n";
  std::cout << "    Cycles:           " << _tick << "\n";
}


template<class v_t, class e_t>
void SimObj::WriteVertexProperty<v_t, e_t>::print_stats_csv() {
  std::cout << _name << "," << _stall_ticks[STALL_CAN_ACCEPT] << ","
    << _stall_ticks[STALL_PROCESSING] << ","
    << _stall_ticks[STALL_PIPE] << ","
    << _stall_ticks[STALL_MEM] << ","
    << _throughput << ",\n";
}


template<class v_t, class e_t>
bool SimObj::WriteVertexProperty<v_t, e_t>::complete() {
  return _complete;
}


template<class v_t, class e_t>
void SimObj::WriteVertexProperty<v_t, e_t>::flush() {
  _complete = false;
}
