/*
 * Andrew Smith
 *
 * WriteTempDstProperty
 *  This is the last stage in the processing phase, it writes the temp value
 *  back to memory and tells the ControlAtomicUpdate module a edge has finished.
 */

#include <cassert>

#include "writeTempDstProperty.h"


SimObj::WriteTempDstProperty::WriteTempDstProperty() {
  _scratchpad = NULL;
  _cau = NULL;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
}


SimObj::WriteTempDstProperty::WriteTempDstProperty(Memory* scratchpad, Module* cau) {
  assert(scratchpad != NULL);
  assert(cau != NULL);
  _scratchpad = scratchpad;
  _ready = false;
  _mem_flag = false;
  _state = OP_WAIT;
}


SimObj::WriteTempDstProperty::~WriteTempDstProperty() {
  // Do Nothing
}


void SimObj::WriteTempDstProperty::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
        // Upstream sent _edge property
        _ready = false;
        next_state = OP_WRITE;
        _stall = STALL_PROCESSING;
      }
      else {
        // Wait for upstream to send _edge
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
      }
      break;
    }
    case OP_WRITE : {
      _mem_flag = false;
      _scratchpad->write(0x01, &_mem_flag);
      _stall = STALL_MEM;
      next_state = OP_MEM_WAIT;
      break;
    }
    case OP_MEM_WAIT : {
      if(_mem_flag) {
        next_state = OP_SIGNAL_CAU;
        _stall = STALL_PROCESSING;
      }
      else {
        next_state = OP_MEM_WAIT;
        _stall = STALL_MEM;
      }
      break;
    }
    case OP_SIGNAL_CAU : {
      _cau->receive_message(MSG_ATOMIC_OP_COMPLETE);
      next_state = OP_WAIT;
      _stall = STALL_CAN_ACCEPT;
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


void SimObj::WriteTempDstProperty::ready(void) {
  _ready = true;
}
