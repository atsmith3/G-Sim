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
SimObj::ReadTempDstProperty<v_t, e_t>::ReadTempDstProperty() {
  _scratchpad = NULL;
  _graph = NULL;
  _scratch_mem = NULL;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
}


template<class v_t, class e_t>
SimObj::ReadTempDstProperty<v_t, e_t>::ReadTempDstProperty(Memory* scratchpad, Utility::readGraph<v_t>* graph, std::map<uint64_t, Utility::pipeline_data<v_t, e_t>>* scratch_mem) {
  assert(scratchpad != NULL);
  assert(graph != NULL);
  assert(scratch_mem != NULL);
  _scratchpad = scratchpad;
  _graph = graph;
  _scratch_mem = scratch_mem;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
}


template<class v_t, class e_t>
SimObj::ReadTempDstProperty<v_t, e_t>::~ReadTempDstProperty() {
  _scratchpad = NULL;
  _graph = NULL;
  _scratch_mem = NULL;
}


template<class v_t, class e_t>
void SimObj::ReadTempDstProperty<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
        // Upstream sent vertex & vertex property
        _ready = false;
        _mem_flag = false;
        _scratchpad->read(0x01, &_mem_flag);
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
        if(_next->is_stalled() == STALL_CAN_ACCEPT) {
          // Read from "scratchpad map holding temp values"
          //Check if it exists in the map:
          if(_scratch_mem->find(_data.vertex_dst_id) != _scratch_mem->end()) {
            _data.vertex_temp_dst_data = _scratch_mem->find(_data.vertex_dst_id)->second.vertex_temp_dst_data;
          }
          else {
            _data.vertex_temp_dst_data = _graph->getInitializer();
          }
          _next->ready(_data);
          next_state = OP_WAIT;
          _stall = STALL_CAN_ACCEPT;
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
#ifdef DEBUG
  if(_state != next_state) {
    std::cout << "[ " << __PRETTY_FUNCTION__ << " ] tick: " << _tick << "  state: " << _state_name[_state] << "  next_state: " << _state_name[next_state] << "\n";
  }
#endif
  _state = next_state;
  this->update_stats();
}

