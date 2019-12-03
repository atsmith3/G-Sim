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
  _logger = new Utility::Log("trace/"+name+"_"+std::to_string(_id)+".csv");
  assert(_logger != NULL);
#endif
}


template<class v_t, class e_t>
SimObj::ReadVertexProperty<v_t, e_t>::~ReadVertexProperty() {
  _dram = NULL;
  _apply = NULL;
  _graph = NULL;
#if MODULE_TRACE
  if(_logger) {
    delete _logger;
    _logger = NULL;
  }
#endif
}


template<class v_t, class e_t>
void SimObj::ReadVertexProperty<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready && !_apply->empty()) {
#ifdef MODULE_TRACE
        // Ready, Complete, Sent
        _logger->write(std::to_string(_tick)+",1,0,0\n");
        mem_req_logged = false;
#endif
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
#ifdef MODULE_TRACE
        if(!mem_req_logged) {
          _logger->write(std::to_string(_tick)+",0,1,0\n");
          mem_req_logged = true;
        }
#endif
        if(_next->is_stalled() == STALL_CAN_ACCEPT) {
#ifdef MODULE_TRACE
        _logger->write(std::to_string(_tick)+",0,0,1\n");
#endif
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
  _state = next_state;
  this->update_stats();
}


template<class v_t, class e_t>
void SimObj::ReadVertexProperty<v_t, e_t>::reset(void) {
  _curr_addr = _base_addr;
}
