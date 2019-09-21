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
  _avg_connectivity = 1;
}


SimObj::ReadSrcEdges::ReadSrcEdges(Memory* scratchpad) {
  assert(scratchpad != NULL);
  _scratchpad = scratchpad;
  _state = OP_WAIT;
  _ready = false;
  _mem_flag = false;
  _avg_connectivity = 1;
}


SimObj::ReadSrcEdges::ReadSrcEdges(Memory* scratchpad, uint64_t avg_connectivity) {
  assert(scratchpad != NULL);
  _scratchpad = scratchpad;
  _state = OP_WAIT;
  _ready = false;
  _mem_flag = false;
  _avg_connectivity = avg_connectivity;
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
        _edge_it = _edge_list.begin();
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
          _next->ready();
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

void SimObj::ReadSrcEdges::ready(void) {
  _ready = true;
  // ToDo Make this an Actual Edge List
  _edge_list.resize(_avg_connectivity);
}
