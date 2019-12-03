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
SimObj::ReadSrcEdges<v_t, e_t>::ReadSrcEdges(Memory* scratchpad, Utility::readGraph<v_t>* graph, std::string name, uint64_t id) {
  assert(scratchpad != NULL);
  assert(graph != NULL);
  _scratchpad = scratchpad;
  _graph = graph;
  _state = OP_WAIT;
  _ready = false;
  _mem_flag = false;
  _name = name;
  _id = id;
#if MODULE_TRACE
  _logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+".csv");
  assert(_logger != NULL);
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

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
        // Upstream sent vertex & vertex property
#ifdef MODULE_TRACE
        // Ready, Complete, Can Accept, Edges
        _logger->write(std::to_string(_tick)+",1,0,0,0\n");
        mem_req_logged = false;
#endif
        _ready = false;
        if(!_edge_list->empty()) {
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
#ifdef MODULE_TRACE
        if(!mem_req_logged) {
          _logger->write(std::to_string(_tick)+",0,1,0,0\n");
          mem_req_logged = true;
        }
#endif
        if(_data_set == false) {
          _data.edge_id = _edge_list->front();
          _edge_list->pop();
          _data.edge_data = _graph->getEdgeWeight(_data.edge_id);
          _data.vertex_dst_id = _graph->getNodeNeighbor(_data.edge_id);
          _data_set = true;
        }
        if(_next->is_stalled(_data) == STALL_CAN_ACCEPT) {
#ifdef MODULE_TRACE
          _logger->write(std::to_string(_tick)+",0,0,1,0\n");
#endif
          if(!_edge_list->empty()) {
#ifdef MODULE_TRACE
            _logger->write(std::to_string(_tick)+",0,0,0,1\n");
            mem_req_logged = false;
#endif
            _data_set = false;
            _mem_flag = false;
            _scratchpad->read(0x01, &_mem_flag);
            _stall = STALL_MEM;
            next_state = OP_MEM_WAIT;
            _data.last_edge = false;
          }
          else {
            delete _edge_list;
            _edge_list = NULL;
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
  _state = next_state;
  this->update_stats();
}

template<class v_t, class e_t>
void SimObj::ReadSrcEdges<v_t, e_t>::ready(Utility::pipeline_data<v_t, e_t> data) {
  _ready = true;
  _has_work = true;
  _data = data;
  _edge_list = _graph->getEdges(data.vertex_id);
#if MODULE_TRACE
  if(_logger != NULL) {
    _logger->write(std::to_string(_tick)+","+
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
                   std::to_string(_data.updated)+"\n");
  }
#endif
}
