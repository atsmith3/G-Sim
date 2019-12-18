/*
 * Andrew Smith
 *
 * Read DST Property:
 *  Read the destination property from DRAM. Optional depending on the 
 *  graph algorithm
 *
 */

#include <cassert>
#include <sstream>

template<class v_t, class e_t>
SimObj::ReadTempDstProperty<v_t, e_t>::ReadTempDstProperty(Memory* scratchpad, std::map<uint64_t, Utility::pipeline_data<v_t, e_t>>* scratch_mem, std::string name, uint64_t id) {
  assert(scratchpad != NULL);
  assert(scratch_mem != NULL);
  _id = id;
  _name = name;
  _scratchpad = scratchpad;
  _scratch_mem = scratch_mem;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
#if MODULE_TRACE
  ready_prev = false;
  mem_flag_prev = false;
  send_prev = false;
  address_prev = 0;
  mem_result_prev = 0;
  ready_curr = false;
  mem_flag_curr = false;
  send_curr = false;
  address_curr = 0;
  mem_result_curr = 0;
  _in_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_in.csv");
  _out_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_out.csv");
  assert(_in_logger != NULL);
  assert(_out_logger != NULL);
#endif
}


template<class v_t, class e_t>
SimObj::ReadTempDstProperty<v_t, e_t>::~ReadTempDstProperty() {
  _scratchpad = NULL;
  _scratch_mem = NULL;
}


template<class v_t, class e_t>
void SimObj::ReadTempDstProperty<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;
#ifdef MODULE_TRACE
  mem_flag_curr = _mem_flag;
  ready_curr = _ready;
  send_curr = _next->is_stalled() == STALL_CAN_ACCEPT;
  address_curr = _data.vertex_dst_id_addr;
#endif

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
        // Upstream sent vertex & vertex property
        _ready = false;
        _mem_flag = false;
        _scratchpad->read(_data.vertex_dst_id_addr, &_mem_flag);
        if(_scratch_mem->find(_data.vertex_dst_id) != _scratch_mem->end()) {
          _data.vertex_temp_dst_data = _scratch_mem->find(_data.vertex_dst_id)->second.vertex_temp_dst_data;
        }
        else {
          _data.vertex_temp_dst_data = v_t();
        }
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
#ifdef MODULE_TRACE
        mem_result_curr = _data.vertex_dst_data;
#endif
        if(_next->is_stalled() == STALL_CAN_ACCEPT) {
          // Read from "scratchpad map holding temp values"
          //Check if it exists in the map:
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
#ifdef MODULE_TRACE
  update_logger();
#endif
  _state = next_state;
  this->update_stats();
}

#ifdef MODULE_TRACE
template<class v_t, class e_t>
void SimObj::ReadTempDstProperty<v_t, e_t>::update_logger(void) {
  std::stringstream out;
  if(ready_prev != ready_curr ||
     mem_flag_prev != mem_flag_curr ||
     send_prev != send_curr ||
     mem_result_prev != mem_result_curr) {
    if(_in_logger != NULL) {
      out << _tick << ",";
      out << _in_data << ",";
      out << ready_curr << ",";
      out << mem_flag_curr << ",";
      out << send_curr << ",";
      out << mem_result_curr << "\n";
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
      //               std::to_string(send_curr)+","+
      //               std::to_string(mem_result_curr)+"\n");
    }
    ready_prev = ready_curr;
    mem_flag_prev = mem_flag_curr;
    send_prev = send_curr;
    mem_result_prev = mem_result_curr;
  }
}
#endif
