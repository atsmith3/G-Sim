/*
 * Andrew Smith
 *
 * WriteTempDstProperty
 *  This is the last stage in the processing phase, it writes the temp value
 *  back to memory and tells the ControlAtomicUpdate module a edge has finished.
 */

#include <cassert>

template<class v_t, class e_t>
SimObj::WriteTempDstProperty<v_t, e_t>::WriteTempDstProperty(Memory* scratchpad, ControlAtomicUpdate<v_t, e_t>* cau, std::map<uint64_t, Utility::pipeline_data<v_t, e_t>>* scratch_mem, std::list<uint64_t>* apply, std::string name, uint64_t id) {
  assert(scratchpad != NULL);
  assert(cau != NULL);
  assert(scratch_mem != NULL);
  assert(apply != NULL);
  _id = id;
  _name = name;
  _apply = apply;
  _scratchpad = scratchpad;
  _cau = cau;
  _scratch_mem = scratch_mem;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
  _edges_written = 0;
#if MODULE_TRACE
  ready_prev = false;
  mem_flag_prev = false;
  address_prev = 0;
  mem_out_prev = 0;
  ready_curr = false;
  mem_flag_curr = false;
  address_curr = 0;
  mem_out_curr = 0;
  _in_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_in.csv");
  _out_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_out.csv");
  assert(_in_logger != NULL);
  assert(_out_logger != NULL);
#endif
}


template<class v_t, class e_t>
SimObj::WriteTempDstProperty<v_t, e_t>::~WriteTempDstProperty() {
  _scratchpad = NULL;
  _scratch_mem = NULL;
  _cau = NULL;
  _apply = NULL;
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
void SimObj::WriteTempDstProperty<v_t, e_t>::tick(void) {
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
        _ready = false;
        _mem_flag = false;
        _scratchpad->write(_data.vertex_dst_id_addr, &_mem_flag);
        _curr_addr += 4;
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
        _scratch_mem->insert_or_assign(_data.vertex_dst_id, _data);
#ifdef MODULE_TRACE
        mem_out_curr = _data.vertex_temp_dst_data;
#endif
        _apply->push_back(_data.vertex_dst_id);
        _edges_written++;
        Utility::pipeline_data<v_t, e_t> temp_data = _cau->signal();
#ifdef DEBUG
        assert(temp_data.vertex_id == _data.vertex_id);
        assert(temp_data.edge_id == _data.edge_id);
        assert(temp_data.vertex_dst_id == _data.vertex_dst_id);
#endif
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
#ifdef MODULE_TRACE
  update_logger();
#endif
  _state = next_state;
  this->update_stats();
}


template<class v_t, class e_t>
void SimObj::WriteTempDstProperty<v_t, e_t>::print_stats(void) {
  sim_out.write("-------------------------------------------------------------------------------\n");
  sim_out.write("[ " + _name + " ]\n");
  sim_out.write("  Stalls:\n");
  sim_out.write("    STALL_CAN_ACCEPT: " + std::to_string(_stall_ticks[STALL_CAN_ACCEPT]) + " cycles\n");
  sim_out.write("    STALL_PROCESSING: " + std::to_string(_stall_ticks[STALL_PROCESSING]) + " cycles\n");
  sim_out.write("    STALL_PIPE:       " + std::to_string(_stall_ticks[STALL_PIPE]) + " cycles\n");
  sim_out.write("    STALL_MEM:        " + std::to_string(_stall_ticks[STALL_MEM]) + " cycles\n");
  sim_out.write("  Performance:\n");
  sim_out.write("    Edges:            " + std::to_string(_edges_written) + "\n");
  sim_out.write("    Cycles:           " + std::to_string(_tick) + "\n");
}


template<class v_t, class e_t>
void SimObj::WriteTempDstProperty<v_t, e_t>::print_stats_csv(void) {
  sim_out.write(_name + "," 
    + std::to_string(_stall_ticks[STALL_CAN_ACCEPT]) + ","
    + std::to_string(_stall_ticks[STALL_PROCESSING]) + ","
    + std::to_string(_stall_ticks[STALL_PIPE]) + ","
    + std::to_string(_stall_ticks[STALL_MEM]) + ","
    + std::to_string(_edges_written) + ","
    + std::to_string(_tick) + "\n");
}


#ifdef MODULE_TRACE
template<class v_t, class e_t>
void SimObj::WriteTempDstProperty<v_t, e_t>::update_logger(void) {
  if(ready_prev != ready_curr ||
     mem_flag_prev != mem_flag_curr) {
    if(_in_logger != NULL) {
      _in_logger->write(std::to_string(_tick)+","+
                     std::to_string(_in_data.vertex_id)+","+
                     std::to_string(_in_data.vertex_id_addr)+","+
                     std::to_string(_in_data.vertex_dst_id)+","+
                     std::to_string(_in_data.vertex_dst_id_addr)+","+
                     std::to_string(_in_data.edge_id)+","+
                     std::to_string(_in_data.vertex_data)+","+
                     std::to_string(_in_data.vertex_dst_data)+","+
                     std::to_string(_in_data.message_data)+","+
                     std::to_string(_in_data.vertex_temp_dst_data)+","+
                     std::to_string(_in_data.edge_data)+","+
                     std::to_string(_in_data.edge_temp_data)+","+
                     std::to_string(_in_data.last_vertex)+","+
                     std::to_string(_in_data.last_edge)+","+
                     std::to_string(_in_data.updated)+","+
                     std::to_string(ready_curr)+","+
                     std::to_string(mem_flag_curr)+"\n");
    }
    ready_prev = ready_curr;
    mem_flag_prev = mem_flag_curr;
  }
  if(address_prev != address_curr ||
     mem_out_prev != mem_out_curr) {
    if(_out_logger != NULL) {
      _out_logger->write(std::to_string(_tick)+","+
                     std::to_string(_data.vertex_id)+","+
                     std::to_string(_data.vertex_id_addr)+","+
                     std::to_string(_data.vertex_dst_id)+","+
                     std::to_string(_data.vertex_dst_id_addr)+","+
                     std::to_string(_data.edge_id)+","+
                     std::to_string(_data.vertex_data)+","+
                     std::to_string(_data.vertex_dst_data)+","+
                     std::to_string(_data.message_data)+","+
                     std::to_string(_data.vertex_temp_dst_data)+","+
                     std::to_string(_data.edge_data)+","+
                     std::to_string(_data.edge_temp_data)+","+
                     std::to_string(_data.last_vertex)+","+
                     std::to_string(_data.last_edge)+","+
                     std::to_string(_data.updated)+","+
                     std::to_string(address_curr)+","+
                     std::to_string(mem_out_curr)+"\n");
    }
    address_prev = address_curr;
    mem_out_prev = mem_out_curr;
  }
}
#endif
