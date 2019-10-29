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
  sequential_write_counter = 0;
  sequential_read_counter = 0;
  buffer_size = 16;
	read_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::read_complete);
	write_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::write_complete);
  _mem = DRAMSim::getMemorySystemInstance("DDR3_micron_64M_8B_x4_sg15.ini", "graphicionado_system.ini", "./modules/memory", "g_sim", 65536);
	_mem->RegisterCallbacks(read_cb, write_cb, NULL);
}

SimObj::DRAM::DRAM(uint64_t access_latency, uint64_t write_latency, uint64_t num_simultaneous_requests) {
  _tick = 0;
  _access_latency = access_latency;
  _write_latency = write_latency;
  sequential_write_counter = 0;
  sequential_read_counter = 0;
  buffer_size = 16;
	read_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::read_complete);
	write_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::write_complete);
  _mem = DRAMSim::getMemorySystemInstance("DDR3_micron_64M_8B_x4_sg15.ini", "graphicionado_system.ini", "./modules/memory", "g_sim", 65536);
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
#if 0
  if(sequential) {
    // check if the sequential read buffer has data:
    if(sequential_write_counter < buffer_size) {
      // Can directly complete the req:
      *complete = true;
      sequential_write_counter++;
      return;
    }
  }
#endif
  if(_mem->addTransaction(true, addr)) {
    std::tuple<uint64_t, bool*, bool> transaction = std::make_tuple(addr, complete, sequential);
    _write_queue.push_front(transaction);
    return;
  }
  assert(1==0); // Shouldnt reach here?
}

void SimObj::DRAM::read(uint64_t addr, bool* complete, bool sequential) {
#ifdef DEBUG
  //std::cout << "DRAM Read  Issued @ " << _tick << " with address: " << std::hex << addr << "\n";
#endif
#if 0
  if(sequential) {
    // check if the sequential read buffer has data:
    if(sequential_read_counter < buffer_size) {
      // Can directly complete the req:
      *complete = true;
      sequential_read_counter++;
      return;
    }
  }
#endif
  if(_mem->addTransaction(false, addr)) {
    std::tuple<uint64_t, bool*, bool> transaction = std::make_tuple(addr, complete, sequential);
    _read_queue.push_front(transaction);
    return;
  }
  assert(1==0); // Shouldnt reach here?
}

void SimObj::DRAM::read_complete(unsigned int id, uint64_t address, uint64_t clock_cycle) {
  //Dequeue the transaction pair from the list of outstanding transactions:
  for(auto it = _read_queue.begin(); it != _read_queue.end(); it++) {
    if(std::get<0>(*it) == address) {
      // Set the complete flag to true
      *(std::get<1>(*it)) = true;
      if(std::get<2>(*it) == true) {
        sequential_read_counter = 0;
      }
      // Remove the transaction from the queue of pending xactions
      _read_queue.erase(it);
      return;
    }
  }
#ifdef DEBUG
  assert(false); 
#endif
}

void SimObj::DRAM::write_complete(unsigned int id, uint64_t address, uint64_t clock_cycle) {
  //Dequeue the transaction pair from the list of outstanding transactions:
  for(auto it = _write_queue.begin(); it != _write_queue.end(); it++) {
    if(std::get<0>(*it) == address) {
      // Set the complete flag to true
      *(std::get<1>(*it)) = true;
      if(std::get<2>(*it) == true) {
        sequential_write_counter = 0;
      }
      // Remove the transaction from the queue of pending xactions
      _write_queue.erase(it);
      return;
    }
  }
#ifdef DEBUG
  assert(false);
#endif
}

void SimObj::DRAM::print_stats() {
  _mem->printStats(true);
}
