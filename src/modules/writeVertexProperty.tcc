/*
 *
 * Andrew Smith
 * 
 * WriteVertexProperty
 *  This is the last stage in the apply phase
 *  Writes the vertex value back to memory
 */

#include <cassert>
#include <sstream>


template<class v_t, class e_t>
SimObj::WriteVertexProperty<v_t, e_t>::WriteVertexProperty(Memory* dram, std::list<uint64_t>* process, Utility::Graph<v_t, e_t>* graph, uint64_t base_addr, std::string name, uint64_t id) {
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
  ready_prev = false;
  mem_flag_prev = false;
  address_prev = 9;
  ready_curr = false;
  mem_flag_curr = false;
  address_curr = _curr_addr;
  _in_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_in.csv");
  _out_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_out.csv");
  assert(_in_logger != NULL);
  assert(_out_logger != NULL);
#endif
}


template<class v_t, class e_t>
SimObj::WriteVertexProperty<v_t, e_t>::~WriteVertexProperty() {
  _dram = NULL;
  _process = NULL;
  _graph = NULL;
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
void SimObj::WriteVertexProperty<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;
#ifdef MODULE_TRACE
  mem_flag_curr = _mem_flag;
  ready_curr = _ready;
  address_curr = _curr_addr;
#endif

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
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
        // Write to global mem
        if(_data.updated) {
          _graph->vertex[_data.vertex_id].property = _data.vertex_data;
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
#ifdef MODULE_TRACE
  update_logger();
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


#ifdef MODULE_TRACE
template<class v_t, class e_t>
void SimObj::WriteVertexProperty<v_t, e_t>::update_logger(void) {
  std::stringstream out;
  if(ready_prev != ready_curr ||
     mem_flag_prev != mem_flag_curr) {
    if(_in_logger != NULL) {
      out << _tick << ",";
      out << _in_data << ",";
      out << ready_curr << ",";
      out << mem_flag_curr << ",";
      out << address_curr << "\n";
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
      //               std::to_string(mem_flag_curr)+","+
      //               std::to_string(address_curr)+"\n");
    }
    ready_prev = ready_curr;
    mem_flag_prev = mem_flag_curr;
    address_prev = address_curr;
  }
}
#endif
