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
	read_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::read_complete);
	write_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::write_complete);
  _mem = DRAMSim::getMemorySystemInstance("DDR3_micron_64M_8B_x4_sg15.ini", "graphicionado_system.ini", "./modules/memory", "g_sim", 65536);
}

SimObj::DRAM::DRAM(uint64_t access_latency, uint64_t write_latency, uint64_t num_simultaneous_requests) {
  _tick = 0;
  _access_latency = access_latency;
  _write_latency = write_latency;
  _num_simultaneous_requests = num_simultaneous_requests;
	read_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::read_complete);
	write_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::write_complete);
  _mem = DRAMSim::getMemorySystemInstance("DDR3_micron_64M_8B_x4_sg15.ini", "graphicionado_system.ini", "./modules/memory", "g_sim", 65536);
}

SimObj::DRAM::~DRAM() {
  delete read_cb;
  delete write_cb;
}

void SimObj::DRAM::tick(void) {
  _tick++;
  _mem->update();
}
  
void SimObj::DRAM::write(uint64_t addr, bool* complete) {
  if(_mem->addTransaction(false, addr)) {
    std::pair<uint64_t, bool*> transaction = std::make_pair(addr, complete);
    _write_queue.push(transaction);
    return;
  }
  assert(1==0); // Shouldnt reach here?
}

void SimObj::DRAM::read(uint64_t addr, bool* complete) {
  if(_mem->addTransaction(false, addr)) {
    std::pair<uint64_t, bool*> transaction = std::make_pair(addr, complete);
    _read_queue.push(transaction);
    return;
  }
  assert(1==0); // Shouldnt reach here?
}

void SimObj::DRAM::read_complete(unsigned int id, uint64_t address, uint64_t clock_cycle) {
  //Dequeue the transaction pair from the list of outstanding transactions:
  std::pair<uint64_t, bool*> transaction = _read_queue.front();
#ifdef DEBUG
  // Verify the addr of the transaction and the DRAM read are the same
  assert(transaction.first == address);
#endif
  _read_queue.pop();
  *transaction.second = true;
}

void SimObj::DRAM::write_complete(unsigned int id, uint64_t address, uint64_t clock_cycle) {
  //Dequeue the transaction pair from the list of outstanding transactions:
  std::pair<uint64_t, bool*> transaction = _write_queue.front();
#ifdef DEBUG
  // Verify the addr of the transaction and the DRAM read are the same
  assert(transaction.first == address);
#endif
  _write_queue.pop();
  *transaction.second = true;
}
