/*
 * Andrew Smith
 *
 * Read SRC Edges:
 *  The Read SRC Edges Module is the second module in the pipeline.
 *  It reads the list of edges of the current vertex from Scratchpad Memory.
 *
 */

#include <cassert>

#include "readSrcEdges.h"


SimObj::ReadSrcEdges::ReadSrcEdges() {
  _scratchpad = NULL;
  _state = OP_WAIT;
  _ready = false;
  _mem_flag = false;
}


SimObj::ReadSrcEdges::ReadSrcEdges(Memory* scratchpad) {
  assert(scratchpad != NULL);
  _scratchpad = scratchpad;
  _state = OP_WAIT;
  _ready = false;
  _mem_flag = false;
}


SimObj::ReadSrcEdges::~ReadSrcEdges() {
  //Do Nothing
}


void SimObj::ReadSrcEdges::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready) {
        // Upstream sent vertex & vertex property
        _ready = false;
        next_state = OP_FETCH;
        _edge_it = _edge_list.begin();
        _stall = STALL_PROCESSING;
      }
      else {
        // Wait for upstream to send vertex
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
      }
      break;
    }
    case OP_FETCH : {
      if(_edge_it != _edge_list.end()) {
        _mem_flag = false;
        _scratchpad->read(0x01, &_mem_flag);
        _stall = STALL_MEM;
        next_state = OP_MEM_WAIT;
        _edge_it++;
      }
      else {
        // Edge List is Empty
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
      }
      break;
    }
    case OP_MEM_WAIT : {
      if(_mem_flag) {
        next_state = OP_SEND_EDGE_DOWNSTREAM;
        _stall = STALL_PROCESSING;
      }
      else {
        next_state = OP_MEM_WAIT;
        _stall = STALL_MEM;
      }
      break;
    }
    case OP_SEND_EDGE_DOWNSTREAM : {
      if(_next->is_stalled() == STALL_CAN_ACCEPT) {
        _next->ready();
        next_state = OP_FETCH;
        _stall = STALL_PROCESSING;
      }
      else {
        next_state = OP_SEND_EDGE_DOWNSTREAM;
        _stall = STALL_PIPE;
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

void SimObj::ReadSrcEdges::ready(void) {
  _ready = true;
  // ToDo Make this an Actual Edge List
  _edge_list.resize(1);
}
