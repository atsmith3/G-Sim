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
SimObj::ReadDstProperty<v_t, e_t>::ReadDstProperty() {
  _dram = NULL;
  _graph = NULL;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
}


template<class v_t, class e_t>
SimObj::ReadDstProperty<v_t, e_t>::ReadDstProperty(Memory* dram, Utility::readGraph<v_t>* graph) {
  assert(dram != NULL);
  assert(graph != NULL);
  _graph = graph;
  _dram = dram;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
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

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
        // Upstream sent vertex & vertex property
#ifdef MODULE_DEBUG
        std::cout << "Tick:" << _tick << " " << _name << " recieved: " << _data << "\n";
#endif
        _ready = false;
        _mem_flag = false;
        _data.vertex_dst_id = _graph->getNodeNeighbor(_data.edge_id);
        _data.vertex_dst_id_addr = _graph->getVertexAddress(_data.vertex_dst_id);
        _dram->read(_data.vertex_dst_id_addr, &_mem_flag);
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
    std::cout << "[ " << __PRETTY_FUNCTION__ << " ] tick: " << _tick << "  state: " << _state_name[_state] << "  next_state: " << _state_name[next_state] << "\n";
  }
#endif
  _state = next_state;
  this->update_stats();
}

