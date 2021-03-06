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
SimObj::ReadSrcEdges<v_t, e_t>::ReadSrcEdges() {
  _scratchpad = NULL;
  _graph = NULL;
  _state = OP_WAIT;
  _ready = false;
  _mem_flag = false;
}


template<class v_t, class e_t>
SimObj::ReadSrcEdges<v_t, e_t>::ReadSrcEdges(Memory* scratchpad, Utility::readGraph<v_t>* graph) {
  assert(scratchpad != NULL);
  assert(graph != NULL);
  _scratchpad = scratchpad;
  _graph = graph;
  _state = OP_WAIT;
  _ready = false;
  _mem_flag = false;
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
#ifdef MODULE_DEBUG
        std::cout << "Tick:" << _tick << " " << _name << " recieved: " << _data << "\n";
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
        if(_data_set == false) {
          _data.edge_id = _edge_list->front();
          _edge_list->pop();
          _data.edge_data = _graph->getEdgeWeight(_data.edge_id);
          _data.vertex_dst_id = _graph->getNodeNeighbor(_data.edge_id);
          _data_set = true;
        }
        if(_next->is_stalled(_data) == STALL_CAN_ACCEPT) {
          if(!_edge_list->empty()) {
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
    std::cout << "[ " << __PRETTY_FUNCTION__ << " ] tick: " << _tick << "  state: " << _state_name[_state] << "  next_state: " << _state_name[next_state] << "\n";
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
}
