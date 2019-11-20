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
	read_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::read_complete);
	write_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::write_complete);
  _mem = DRAMSim::getMemorySystemInstance("DDR4_micro_32M_16B_x8.ini", "graphicionado_system.ini", "./modules/memory", "g_sim", 65536);
	_mem->RegisterCallbacks(read_cb, write_cb, NULL);
}

SimObj::DRAM::DRAM(uint64_t access_latency, uint64_t write_latency, uint64_t num_simultaneous_requests) {
  _tick = 0;
  _access_latency = access_latency;
  _write_latency = write_latency;
	read_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::read_complete);
	write_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::write_complete);
  _mem = DRAMSim::getMemorySystemInstance("DDR4_micro_32M_16B_x8.ini", "graphicionado_system.ini", "./modules/memory", "g_sim", 65536);
	_mem->RegisterCallbacks(read_cb, write_cb, NULL);
}

SimObj::DRAM::~DRAM() {
  delete read_cb;
  delete write_cb;
}

void SimObj::DRAM::tick(void) {
  _tick++;
  _mem->update();
}
  
void SimObj::DRAM::write(uint64_t addr, bool* complete, bool sequential) {
#ifdef DEBUG
  //std::cout << "DRAM Write Issued @ " << _tick << " with address: " << std::hex << addr << "\n";
#endif
  std::tuple<uint64_t, bool*, bool> transaction = std::make_tuple(addr, complete, sequential);
  if(_mem->addTransaction(true, addr)) {
    _write_queue.push_back(transaction);
    return;
  }
  assert(1==0); // Shouldnt reach here?
}

void SimObj::DRAM::read(uint64_t addr, bool* complete, bool sequential) {
#ifdef DEBUG
  //std::cout << "DRAM Read  Issued @ " << _tick << " with address: " << std::hex << addr << "\n";
#endif
  std::tuple<uint64_t, bool*, bool> transaction = std::make_tuple(addr, complete, sequential);
  if(_mem->addTransaction(false, addr)) {
    _read_queue.push_back(transaction);
    return;
  }
  assert(1==0); // Shouldnt reach here?
}

void SimObj::DRAM::read_complete(unsigned int id, uint64_t address, uint64_t clock_cycle) {
  for(auto it = _read_queue.begin(); it != _read_queue.end(); it++) {
    if(std::get<0>(*it) == address) {
      *(std::get<1>(*it)) = true;
      it = _read_queue.erase(it);
      return;
    }
  }
  assert(false); 
}

void SimObj::DRAM::write_complete(unsigned int id, uint64_t address, uint64_t clock_cycle) {
  //Dequeue the transaction pair from the list of outstanding transactions:
  for(auto it = _write_queue.begin(); it != _write_queue.end(); it++) {
    if(std::get<0>(*it) == address) {
      *(std::get<1>(*it)) = true;
      it = _write_queue.erase(it);
      return;
    }
  }
  assert(false);
}

void SimObj::DRAM::print_stats() {
  _mem->printStats(true);
}

void SimObj::DRAM::reset() {

}
