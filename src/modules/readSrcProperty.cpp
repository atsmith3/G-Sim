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

#include "readSrcProperty.h"

SimObj::ReadSrcProperty::ReadSrcProperty() {
  _dram = NULL;
  _state = OP_WAIT;
  _mem_flag = false;
  _fetched = false;
}


SimObj::ReadSrcProperty::ReadSrcProperty(Memory* dram) {
  assert(dram != NULL);
  _dram = dram;
  _state = OP_WAIT;
  _mem_flag = false;
  _fetched = false;
}


SimObj::ReadSrcProperty::~ReadSrcProperty() {
  _dram = NULL;
}


void SimObj::ReadSrcProperty::tick() {
  _tick++;
  read_src_op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_fetched && _next->is_stalled() == STALL_CAN_ACCEPT) {
        // Send Data Downstream
        _next->ready();
        _fetched = false;
        next_state = OP_WAIT;
        _stall = STALL_PROCESSING;
      }
      else if(!_fetched) {
        // Data Not Fetched yet, Begin Fetch Sequence
        _mem_flag = false;
        _dram->read(0x100, &_mem_flag);
        _stall = STALL_MEM;
        next_state = OP_MEM_WAIT;
      }
      else {
        // Data Fetched, but Cannot send Downstream
        next_state = OP_WAIT;
        _stall = STALL_PIPE;
      }
      break;
    }
    case OP_MEM_WAIT : {
      if(_mem_flag) {
        if(_next->is_stalled() == STALL_CAN_ACCEPT) {
          _next->ready();
          _fetched = false;
          _mem_flag = false;
          _dram->read(0x100, &_mem_flag);
          _stall = STALL_MEM;
          next_state = OP_MEM_WAIT;
        }
        else {
          next_state = OP_WAIT;
          _stall = STALL_PIPE;
          _fetched = true;
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


void SimObj::ReadSrcProperty::ready() {
  // Not Needed
}

