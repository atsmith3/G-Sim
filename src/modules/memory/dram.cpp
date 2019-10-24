/*
 * Andrew Smith
 * 
 * DRAM Memory. Derived from Memory class, Uses DRAMSim2.
 *
 */

#include <iostream>
#include <cassert>

#include "dram.h"

SimObj::DRAM::DRAM() {
  _tick = 0;
  _access_latency = 0;
  _write_latency = 0;
  _num_simultaneous_requests = 1;
  _mem = DRAMSim::getMemorySystemInstance("DDR3_micron_16M_8B_x8_sg15.ini", "graphicionado_system.ini", ".", "g_sim", 16384);
  _action.resize(_num_simultaneous_requests);
}

SimObj::DRAM::DRAM(uint64_t access_latency, uint64_t write_latency, uint64_t num_simultaneous_requests) {
  _tick = 0;
  _access_latency = access_latency;
  _write_latency = write_latency;
  _num_simultaneous_requests = num_simultaneous_requests;
  _mem = DRAMSim::getMemorySystemInstance("DDR3_micron_16M_8B_x8_sg15.ini", "graphicionado_system.ini", ".", "g_sim", 16384);
  assert(_num_simultaneous_requests >= 1);
  _action.resize(_num_simultaneous_requests);
}

SimObj::DRAM::~DRAM() {
  // Do Nothing
}

void SimObj::DRAM::tick(void) {
  _tick++;
  _mem->update();
}
  
void SimObj::DRAM::write(uint64_t addr, bool* complete) {
  SimObj::MemRequest req(complete, _access_latency + _write_latency, MEM_WRITE);
  _req_queue.push(1);
}

void SimObj::DRAM::read(uint64_t addr, bool* complete) {
  SimObj::MemRequest req(complete, _access_latency, MEM_READ);
  _req_queue.push(1);
}
