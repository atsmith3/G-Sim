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
SimObj::WriteVertexProperty<v_t, e_t>::WriteVertexProperty(Memory* dram, std::list<uint64_t>* process, Utility::readGraph<v_t>* graph, uint64_t base_addr, std::string name, uint64_t id) {
  assert(dram != NULL);
  assert(graph != NULL);
  assert(process != NULL);
  _id = id;
  _name = name;
  _graph = graph;
  _process = process;
  _dram = dram;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
  _throughput = 0;
  _base_addr = base_addr;
  _curr_addr = base_addr;
#if MODULE_TRACE
  _logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+".csv");
  assert(_logger != NULL);
#endif
}


template<class v_t, class e_t>
SimObj::WriteVertexProperty<v_t, e_t>::~WriteVertexProperty() {
  _dram = NULL;
  _process = NULL;
  _graph = NULL;
#if MODULE_TRACE
  if(_logger) {
    delete _logger;
    _logger = NULL;
  }
#endif
}


template<class v_t, class e_t>
void SimObj::WriteVertexProperty<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
#ifdef MODULE_TRACE
        // Ready, Complete
        _logger->write(std::to_string(_tick)+",1,0\n");
#endif
        // Upstream sent _edge property
        _ready = false;
        _mem_flag = false;
        _dram->write(_curr_addr, &_mem_flag);
        _stall = STALL_MEM;
        next_state = OP_MEM_WAIT;
      }
      else {
        // Wait for upstream to send _edge
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
        _has_work = false;
      }
      break;
    }
    case OP_MEM_WAIT : {
      if(_mem_flag) {
#ifdef MODULE_TRACE
        _logger->write(std::to_string(_tick)+",0,1\n");
#endif
        // Write to global mem
        if(_data.updated) {
          _graph->setVertexProperty(_data.vertex_id, _data.vertex_data);
          _process->push_back(_data.vertex_id);
          _throughput++;
        }
        _curr_addr += 4;
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
    sim_out.write("[ " + (std::string)__PRETTY_FUNCTION__ + " ] tick: " + std::to_string(_tick) + "  state: " + _state_name[_state] + "  next_state: " + _state_name[next_state] + "\n");
  }
#endif
  _state = next_state;
  this->update_stats();
}


template<class v_t, class e_t>
void SimObj::WriteVertexProperty<v_t, e_t>::reset(void) {
  _curr_addr = _base_addr;
}


template<class v_t, class e_t>
void SimObj::WriteVertexProperty<v_t, e_t>::print_stats(void) {
  sim_out.write("-------------------------------------------------------------------------------\n");
  sim_out.write("[ " + _name + " ]\n");
  sim_out.write("  Stalls:\n");
  sim_out.write("    STALL_CAN_ACCEPT: " + std::to_string(_stall_ticks[STALL_CAN_ACCEPT]) + " cycles\n");
  sim_out.write("    STALL_PROCESSING: " + std::to_string(_stall_ticks[STALL_PROCESSING]) + " cycles\n");
  sim_out.write("    STALL_PIPE:       " + std::to_string(_stall_ticks[STALL_PIPE]) + " cycles\n");
  sim_out.write("    STALL_MEM:        " + std::to_string(_stall_ticks[STALL_MEM]) + " cycles\n");
  sim_out.write("  Performance:\n");
  sim_out.write("    Vertex:           " + std::to_string(_throughput) + "\n");
  sim_out.write("    Cycles:           " + std::to_string(_tick) + "\n");
}


template<class v_t, class e_t>
void SimObj::WriteVertexProperty<v_t, e_t>::print_stats_csv() {
  sim_out.write(_name + "," 
    + std::to_string(_stall_ticks[STALL_CAN_ACCEPT]) + ","
    + std::to_string(_stall_ticks[STALL_PROCESSING]) + ","
    + std::to_string(_stall_ticks[STALL_PIPE]) + ","
    + std::to_string(_stall_ticks[STALL_MEM]) + ","
    + std::to_string(_throughput) + ","
    + std::to_string(_tick) + "\n");
}

