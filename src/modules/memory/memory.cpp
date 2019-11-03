/*
 * Andrew Smith
 *
 * Memory Base Class
 *
 */

#include <iostream>
#include <cassert>

#include "memory.h"
  
SimObj::MemRequest::MemRequest() {
  _complete = NULL;
  _start = 0;
  _delay = 0;
  _active = false;
  _type = MEM_READ;
}

SimObj::MemRequest::MemRequest(bool* complete, uint64_t delay, mem_op_t type) {
  _complete = complete;
  _start = 0;
  _delay = delay;
  _active = false;
  _type = type;
}

SimObj::MemRequest::~MemRequest() {
  _complete = NULL;
}

void SimObj::MemRequest::set_start(uint64_t start) {
  _start = start;
}

void SimObj::MemRequest::set_status(bool active) {
  _active = active;
}

bool SimObj::MemRequest::get_status(void) {
  return _active;
}

uint64_t SimObj::MemRequest::get_finish_tick(void) {
  return _start + _delay;
}

void SimObj::MemRequest::complete(void) {
  assert(_complete != NULL);
  *_complete = true;
}

SimObj::Memory::Memory() {
  _tick = 0;
  _access_latency = 0;
  _write_latency = 0;
  _num_simultaneous_requests = 1;
  _action.resize(_num_simultaneous_requests);
}

SimObj::Memory::Memory(uint64_t access_latency, uint64_t write_latency, uint64_t num_simultaneous_requests) {
  _tick = 0;
  _access_latency = access_latency;
  _write_latency = write_latency;
  _num_simultaneous_requests = num_simultaneous_requests;
  assert(_num_simultaneous_requests >= 1);
  _action.resize(_num_simultaneous_requests);
}

SimObj::Memory::~Memory() {
  // Do Nothing
}

void SimObj::Memory::tick(void) {
  _tick++;
  for(auto it = _action.begin(); it != _action.end(); it++) {
    // Update Requests:
    if(it->get_status()) {
      if(it->get_finish_tick() <= _tick) {
        it->set_status(false);
        it->complete();
      }
    }
    else {
      if(!_req_queue.empty()) {
        *it = _req_queue.front();
        _req_queue.pop();
        it->set_status(true);
        it->set_start(_tick);
      }
    }
  }
}
  
void SimObj::Memory::write(uint64_t addr, bool* complete, bool sequential) {
  SimObj::MemRequest req(complete, _access_latency + _write_latency, MEM_WRITE);
  _req_queue.push(req);
}

void SimObj::Memory::read(uint64_t addr, bool* complete, bool sequential) {
  SimObj::MemRequest req(complete, _access_latency, MEM_READ);
  _req_queue.push(req);
}

void SimObj::Memory::print_stats() {

}

void SimObj::Memory::reset() {
  // Do Nothing
}
