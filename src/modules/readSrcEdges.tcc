/*
 * Andrew Smith
 *
 * Read SRC Edges:
 *  The Read SRC Edges Module is the second module in the pipeline.
 *  It reads the list of edges of the current vertex from Scratchpad Memory.
 *
 */

#include <cassert>

template<class v_t, class e_t>
SimObj::ReadSrcEdges<v_t, e_t>::ReadSrcEdges(Memory* scratchpad, Utility::Graph<v_t, e_t>* graph, std::string name, uint64_t id) {
  assert(scratchpad != NULL);
  assert(graph != NULL);
  _scratchpad = scratchpad;
  _graph = graph;
  _state = OP_WAIT;
  _ready = false;
  _mem_flag = false;
  _name = name;
  _id = id;
  _curr_addr = 0x1000;
#if MODULE_TRACE
  ready_prev = false;
  mem_flag_prev = false;
  send_prev = false;
  edges_empty_prev = false;
  address_prev = _curr_addr;
  edge_data_prev = e_t();
  dst_id_prev = 0;
  num_edges_prev = 0;
  ready_curr = false;
  mem_flag_curr = false;
  send_curr = false;
  edges_empty_curr = false;
  address_curr = _curr_addr;
  edge_data_curr = e_t();
  dst_id_curr = 0;
  num_edges_curr = 0;
  _in_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_in.csv");
  _out_logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_out.csv");
  assert(_in_logger != NULL);
  assert(_out_logger != NULL);
#endif
}


template<class v_t, class e_t>
SimObj::ReadSrcEdges<v_t, e_t>::~ReadSrcEdges() {
  _scratchpad = NULL;
  _graph = NULL;
}


template<class v_t, class e_t>
void SimObj::ReadSrcEdges<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;
#ifdef MODULE_TRACE
  mem_flag_curr = _mem_flag;
  ready_curr = _ready;
  send_curr = _next->is_stalled() == STALL_CAN_ACCEPT;
  address_curr = _curr_addr;
#endif

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
        // Upstream sent vertex & vertex property
        _ready = false;
        if(_edge_list.size() != 0) {
          _data_set = false;
          _mem_flag = false;
          _scratchpad->read(0x01, &_mem_flag);
          _stall = STALL_MEM;
          next_state = OP_MEM_WAIT;
        }
        else {
          // Edge List is Empty
          next_state = OP_WAIT;
          _stall = STALL_CAN_ACCEPT;
        }
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
        if(_data_set == false) {
          _data.edge_id =_edge_list.size() - 1;
          _data.edge_data = _edge_list[_edge_list.size() - 1].property;
          _data.vertex_dst_id = _edge_list[_edge_list.size() - 1].dst;
          _edge_list.pop_back();
          _data_set = true;
#ifdef MODULE_TRACE
          edge_data_curr = _data.edge_data;
          dst_id_curr = _data.vertex_dst_id;
#endif
        }
        if(_next->is_stalled(_data) == STALL_CAN_ACCEPT) {
          if(_edge_list.size() != 0) {
            _data_set = false;
            _mem_flag = false;
            _scratchpad->read(_curr_addr, &_mem_flag);
            _curr_addr += 4;
            _stall = STALL_MEM;
            next_state = OP_MEM_WAIT;
            _data.last_edge = false;
          }
          else {
            next_state = OP_WAIT;
            _stall = STALL_CAN_ACCEPT;
            _data.last_edge = true;
            _has_work = false;
          }
          _next->ready(_data);
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


template<class v_t, class e_t>
void SimObj::ReadSrcEdges<v_t, e_t>::ready(Utility::pipeline_data<v_t, e_t> data) {
  _ready = true;
  _has_work = true;
  _data = data;
  _edge_list = _graph->vertex[data.vertex_id].edges;
#if MODULE_TRACE
  num_edges_curr = _edge_list->size();
  _curr_addr = 0x1000;
  _in_data = data;
#endif
}


#ifdef MODULE_TRACE
template<class v_t, class e_t>
void SimObj::ReadSrcEdges<v_t, e_t>::update_logger(void) {
  if(ready_prev != ready_curr ||
     mem_flag_prev != mem_flag_curr ||
     send_prev != send_curr ||
     edges_empty_prev != edges_empty_curr ||
     edge_data_prev != edge_data_curr ||
     dst_id_prev != dst_id_curr) {
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
                     std::to_string(edges_empty_curr)+","+
                     std::to_string(edge_data_curr)+","+
                     std::to_string(dst_id_curr)+","+
                     std::to_string(address_curr)+","+
                     std::to_string(num_edges_curr)+"\n");
    }
    ready_prev = ready_curr;
    mem_flag_prev = mem_flag_curr;
    send_prev = send_curr;
    edges_empty_prev = edges_empty_curr;
    edge_data_prev = edge_data_curr;
    dst_id_prev = dst_id_curr;
    address_prev = address_curr;
    num_edges_prev = num_edges_curr;
  }
}
#endif
