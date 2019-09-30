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
SimObj::WriteVertexProperty::WriteVertexProperty() {
  _dram = NULL;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
  _throughput = 0;
}


template<class v_t, class e_t>
SimObj::WriteVertexProperty::WriteVertexProperty(Memory* dram, Utility::readGraph<v_t>* graph, std::queue<uint64_t>* processs) {
  assert(dram != NULL);
  assert(graph != NULL);
  assert(process != NULL);
  _dram = dram;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
  _throughput = 0;
}


template<class v_t, class e_t>
SimObj::WriteVertexProperty::~WriteVertexProperty() {
  // Do Nothing
}


template<class v_t, class e_t>
void SimObj::WriteVertexProperty::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
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
        _graph->setVertexProperty(_data.vertex_id, _data.vertex_data);
        _process->push(_data.vertex_id);
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
        _throughput++;
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
    std::cout << "[ " << __PRETTY_FUNCTION__ << " ] tick: " << _tick << "  state: " << _state_name[_state] << "  next_state: " << _state_name[next_state] << "\n";
  }
#endif
  _state = next_state;
  update_stats();
}


template<class v_t, class e_t>
void SimObj::WriteVertexProperty::print_stats(void) {
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
void SimObj::WriteVertexProperty::print_stats_csv() {
  std::cout << _name << "," << _stall_ticks[STALL_CAN_ACCEPT] << ","
    << _stall_ticks[STALL_PROCESSING] << ","
    << _stall_ticks[STALL_PIPE] << ","
    << _stall_ticks[STALL_MEM] << ","
    << _throughput << ",\n";
}
