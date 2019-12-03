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
  _logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+"_"+std::to_string(_rid)+".csv");
  assert(_logger != NULL);
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

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
        // Upstream sent vertex & vertex property
#ifdef MODULE_TRACE
        // Ready, Complete, Send
        _logger->write(std::to_string(_tick)+",1,0,0\n");
        mem_req_logged = false;
#endif
        _ready = false;
        _mem_flag = false;
        _data.vertex_dst_id = _graph->getNodeNeighbor(_data.edge_id);
        _data.vertex_dst_id_addr = _graph->getVertexAddress(_data.vertex_dst_id);
        _dram->read(_data.vertex_dst_id_addr, &_mem_flag, false);
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
        if(!mem_req_logged) {
          _logger->write(std::to_string(_tick)+",0,1,0\n");
          mem_req_logged = true;
        }
#endif
        _data.vertex_dst_data = _graph->getVertexProperty(_data.vertex_dst_id);
        if(_next->is_stalled() == STALL_CAN_ACCEPT) {
#ifdef MODULE_TRACE
        _logger->write(std::to_string(_tick)+",0,0,1\n");
#endif
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
  _state = next_state;
  this->update_stats();
}

