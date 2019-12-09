/*
 * Andrew Smith
 *
 * Read DST Property:
 *  Read the destination property from DRAM. Optional depending on the 
 *  graph algorithm
 *
 */

#include <cassert>

template<class v_t, class e_t>
SimObj::ReadDstProperty<v_t, e_t>::ReadDstProperty(Memory* dram, Utility::readGraph<v_t>* graph, std::string name, uint64_t id, uint64_t reader_id) {
  assert(dram != NULL);
  assert(graph != NULL);
  _id = id;
  _rid = reader_id;
  _name = name;
  _graph = graph;
  _dram = dram;
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
  _in_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_"+std::to_string(_rid)+"_in.csv");
  _out_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_"+std::to_string(_rid)+"_out.csv");
  assert(_in_logger != NULL);
  assert(_out_logger != NULL);
#endif
}


template<class v_t, class e_t>
SimObj::ReadDstProperty<v_t, e_t>::~ReadDstProperty() {
  _graph = NULL;
  _dram = NULL;
}


template<class v_t, class e_t>
void SimObj::ReadDstProperty<v_t, e_t>::tick(void) {
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
      if(_ready) {
        // Upstream sent vertex & vertex property
        _ready = false;
        _mem_flag = false;
        _data.vertex_dst_id = _graph->getNodeNeighbor(_data.edge_id);
        _data.vertex_dst_id_addr = _graph->getVertexAddress(_data.vertex_dst_id);
        _dram->read(_data.vertex_dst_id_addr, &_mem_flag, false);
#ifdef MODULE_TRACE
        address_curr = _data.vertex_dst_id_addr;
#endif
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
        _data.vertex_dst_data = _graph->getVertexProperty(_data.vertex_dst_id);
#ifdef MODULE_TRACE
        mem_result_curr = _data.vertex_dst_data;
#endif
        if(_next->is_stalled() == STALL_CAN_ACCEPT) {
          _next->ready(_data);
          _stall = STALL_CAN_ACCEPT;
          next_state = OP_WAIT;
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
    _logger->write(std::to_string(_tick)+","+std::to_string((uint64_t)_state)+","+_state_name[_state]+","+std::to_string((uint64_t)next_state)+","+_state_name[next_state]+"\n");
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
void SimObj::ReadDstProperty<v_t, e_t>::update_logger(void) {
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
                     std::to_string(mem_result_curr)+"\n");
    }
    ready_prev = ready_curr;
    mem_flag_prev = mem_flag_curr;
    send_prev = send_curr;
    mem_result_prev = mem_result_curr;
  }
}
#endif
