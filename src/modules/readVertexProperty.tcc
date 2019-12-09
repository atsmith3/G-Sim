/*
 * Andrew Smith
 *
 * Read Vertex Property:
 *  Read the Vertex property from DRAM.
 */

#include <cassert>


template<class v_t, class e_t>
SimObj::ReadVertexProperty<v_t, e_t>::ReadVertexProperty(Memory* dram, std::list<uint64_t>* apply, Utility::readGraph<v_t>* graph, uint64_t base_addr, std::string name, uint64_t id) {
  assert(dram != NULL);
  assert(apply != NULL);
  assert(graph != NULL);
  _id = id;
  _name = name;
  _apply = apply;
  _graph = graph;
  _dram = dram;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
  _base_addr = base_addr;
  _curr_addr = base_addr;
#if MODULE_TRACE
  ready_prev = false;
  mem_flag_prev = false;
  send_prev = false;
  address_prev = 0;
  mem_result_prev = 0;
  queue_length_prev = 0;
  iteration_reset_prev = false;
  ready_curr = false;
  mem_flag_curr = false;
  send_curr = false;
  address_curr = (uint64_t)_apply;
  mem_result_curr = 0;
  queue_length_curr = _apply->size();
  iteration_reset_curr = true;
  _in_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_in.csv");
  _out_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_out.csv");
  assert(_in_logger != NULL);
  assert(_out_logger != NULL);
#endif
}


template<class v_t, class e_t>
SimObj::ReadVertexProperty<v_t, e_t>::~ReadVertexProperty() {
  _dram = NULL;
  _apply = NULL;
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
void SimObj::ReadVertexProperty<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;
#ifdef MODULE_TRACE
  mem_flag_curr = _mem_flag;
  ready_curr = _ready;
  send_curr = _next->is_stalled() == STALL_CAN_ACCEPT;
#endif

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
#ifdef MODULE_TRACE
        mem_result_curr = _data.vertex_data;
#endif
        _mem_flag = false;
        _dram->read(_curr_addr, &_mem_flag);
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
          _curr_addr += 4;
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
    sim_out.write("[ " + (std::string)__PRETTY_FUNCTION__ + " ] tick: " + std::to_string(_tick) + "  state: " + _state_name[_state] + "  next_state: " + _state_name[next_state] + "\n");
  }
#endif
#ifdef MODULE_TRACE
  update_logger();
  queue_length_curr = 0;
  iteration_reset_curr = false;
#endif
  _state = next_state;
  this->update_stats();
}


template<class v_t, class e_t>
void SimObj::ReadVertexProperty<v_t, e_t>::reset(void) {
  _curr_addr = _base_addr;
#ifdef MODULE_TRACE
  queue_length_curr = _apply->size();
  iteration_reset_curr = true;
#endif
}


#ifdef MODULE_TRACE
template<class v_t, class e_t>
void SimObj::ReadVertexProperty<v_t, e_t>::update_logger(void) {
  if(ready_prev != ready_curr ||
     mem_flag_prev != mem_flag_curr ||
     send_prev != send_curr ||
     mem_result_prev != mem_result_curr) {
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
                     std::to_string(mem_flag_curr)+","+
                     std::to_string(send_curr)+","+
                     std::to_string(mem_result_curr)+","+
                     std::to_string(address_curr)+","+
                     std::to_string(queue_length_curr)+","+
                     std::to_string(iteration_reset_curr)+"\n");
    }
    ready_prev = ready_curr;
    mem_flag_prev = mem_flag_curr;
    send_prev = send_curr;
    mem_result_prev = mem_result_curr;
    address_prev = address_curr;
    queue_length_prev = queue_length_curr;
    iteration_reset_prev = iteration_reset_curr;
  }
}
#endif
