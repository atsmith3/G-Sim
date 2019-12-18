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
#include <sstream>

template<class v_t, class e_t>
SimObj::ReadSrcProperty<v_t, e_t>::ReadSrcProperty(Memory* dram, std::list<uint64_t>* process, Utility::Graph<v_t, e_t>* graph, uint64_t base_addr, std::string name, uint64_t id) {
  assert(dram != NULL);
  assert(process != NULL);
  assert(graph != NULL);
  _id = id;
  _name = name;
  _graph = graph;
  _dram = dram;
  _process = process;
  _state = OP_WAIT;
  _mem_flag = false;
  _fetched = false;
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
  address_curr = (uint64_t)_process;
  mem_result_curr = 0;
  queue_length_curr = _process->size();
  iteration_reset_curr = true;
  _in_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_in.csv");
  _out_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_out.csv");
  assert(_in_logger != NULL);
  assert(_out_logger != NULL);
#endif
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
#ifdef MODULE_TRACE
  mem_flag_curr = _mem_flag;
  ready_curr = _ready;
  send_curr = _next->is_stalled() == STALL_CAN_ACCEPT;
  address_curr = (uint64_t)_process;
#endif

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready && !_process->empty()) {
        // Dequeue from the work queue
        _data.vertex_id = _process->front();
        _process->pop_front();
        _data.vertex_id_addr = _curr_addr;
        //_data.vertex_id_addr = &_graph->vertex[_data.vertex_id];
        _data.vertex_data = _graph->vertex[_data.vertex_id].property;
#ifdef MODULE_TRACE
        mem_result_curr = _data.vertex_data;
#endif
        _data.edge_id = 0;
        _items_processed++;

        if(_process->empty()) {
          _data.last_vertex = true;
          _ready = false;
        }
        else {
          _data.last_vertex = false;
        }

        _mem_flag = false;
        _dram->read(_curr_addr, &_mem_flag);
        _stall = STALL_MEM;
        next_state = OP_MEM_WAIT;
      }
      else {
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
  queue_length_curr = 0;
  iteration_reset_curr = false;
#endif
  _state = next_state;
  this->update_stats();
}


template<class v_t, class e_t>
void SimObj::ReadSrcProperty<v_t, e_t>::reset(void) {
  _curr_addr = _base_addr;
#ifdef MODULE_TRACE
  queue_length_curr = _process->size();
  iteration_reset_curr = true;
#endif
}

#ifdef MODULE_TRACE
template<class v_t, class e_t>
void SimObj::ReadSrcProperty<v_t, e_t>::update_logger(void) {
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
      out << mem_result_curr << ",";
      out << address_curr << ",";
      out << queue_length_curr << ",";
      out << iteration_reset_curr << "\n";
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
      //               std::to_string(mem_result_curr)+","+
      //               std::to_string(address_curr)+","+
      //               std::to_string(queue_length_curr)+","+
      //               std::to_string(iteration_reset_curr)+"\n");
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
