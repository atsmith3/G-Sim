/*
 * Andrew Smith
 *
 * Read DST Property:
 *  Read the destination property from DRAM. Optional depending on the 
 *  graph algorithm
 *
 */

#include <cassert>

#include "readTempDstProperty.h"


SimObj::ReadTempDstProperty::ReadTempDstProperty() {
  _scratchpad = NULL;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
}


SimObj::ReadTempDstProperty::ReadTempDstProperty(Memory* scratchpad) {
  assert(scratchpad != NULL);
  _scratchpad = scratchpad;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
}


SimObj::ReadTempDstProperty::~ReadTempDstProperty() {
  // Do Nothing
}


void SimObj::ReadTempDstProperty::tick(void) {
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
          _next->ready(_vertex_id, _edge_id);
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
  update_stats();
}


void SimObj::ReadTempDstProperty::ready(void) {
  _ready = true;
}
