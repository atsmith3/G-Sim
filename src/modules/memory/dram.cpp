/*
 * Andrew Smith
 * 
 * DRAM Memory. Derived from Memory class, Uses DRAMSim2.
 *
 */

#include <iostream>
#include <cassert>

#include "dram.h"

#define DRAM_BUFFER

#define DRAM_ACCESS_WIDTH 0x10
#define SEQUENTIAL_START_ADDR 0xdeadbee0

SimObj::DRAM::DRAM() {
  _tick = 0;
  _access_latency = 0;
  _write_latency = 0;
  sequential_write_counter = 0;
  sequential_read_counter = 0;
  sequential_read_addr = SEQUENTIAL_START_ADDR;
  sequential_write_addr = SEQUENTIAL_START_ADDR;
  buffer_size = 16;
	read_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::read_complete);
	write_cb = new DRAMSim::Callback<SimObj::DRAM, void, unsigned, uint64_t, uint64_t>(this, &SimObj::DRAM::write_complete);
  _mem = DRAMSim::getMemorySystemInstance("DDR4_micro_32M_16B_x8.ini", "graphicionado_system.ini", "./modules/memory", "g_sim", 65536);
	_mem->RegisterCallbacks(read_cb, write_cb, NULL);
}

SimObj::DRAM::DRAM(uint64_t access_latency, uint64_t write_latency, uint64_t num_simultaneous_requests) {
  _tick = 0;
  _access_latency = access_latency;
  _write_latency = write_latency;
  sequential_write_counter = 0;
  sequential_read_counter = 0;
  sequential_read_addr = SEQUENTIAL_START_ADDR;
  sequential_write_addr = SEQUENTIAL_START_ADDR;
  buffer_size = 16;
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
#ifdef DRAM_BUFFER
  std::tuple<uint64_t, bool*, bool> transaction;
  if(sequential_read_counter < buffer_size) {
    for(int i = sequential_read_counter; i < buffer_size; i++) {
      if(!_sequential_read_queue.empty()) {
        // Dequeue and mark as complete:
        sequential_read_counter++;
        transaction = _sequential_read_queue.front();
        _sequential_read_queue.pop_front();
        *(std::get<1>(transaction)) = true;
      }
      else {
        break;
      }
    }
  }
  else if (!_sequential_read_queue.empty() && !sequential_read_issued) {
    // Issue a new sequential Read:
    _mem->addTransaction(false, sequential_read_addr);
    transaction = std::make_tuple(sequential_read_addr, &_complete, true);
    _read_queue.push_back(transaction);
    sequential_read_addr += buffer_size;
    sequential_read_issued = true;
  }
  if(sequential_write_counter < buffer_size) {
    for(int i = sequential_write_counter; i < buffer_size; i++) {
      if(!_sequential_write_queue.empty()) {
        // Dequeue and mark as complete:
        sequential_write_counter++;
        transaction = _sequential_write_queue.front();
        _sequential_write_queue.pop_front();
        *(std::get<1>(transaction)) = true;
      }
      else {
        break;
      }
    }
  }
  else if (!_sequential_write_queue.empty() && !sequential_write_issued) {
    // Issue a new sequential Read:
    _mem->addTransaction(true, sequential_write_addr);
    transaction = std::make_tuple(sequential_write_addr, &_complete, true);
    _write_queue.push_back(transaction);
    sequential_write_addr += buffer_size;
    sequential_write_issued = true;
  }
#endif
  _mem->update();
}
  
void SimObj::DRAM::write(uint64_t addr, bool* complete, bool sequential) {
#ifdef DEBUG
  //std::cout << "DRAM Write Issued @ " << _tick << " with address: " << std::hex << addr << "\n";
#endif
  std::tuple<uint64_t, bool*, bool> transaction = std::make_tuple(addr, complete, sequential);
#ifdef DRAM_BUFFER
  if(sequential) {
    // Add to the Sequential Queue:
    _sequential_write_queue.push_back(transaction);
    return;
  }
#endif
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
#ifdef DRAM_BUFFER
  if(sequential) {
    // Add to the Sequential Queue:
    _sequential_read_queue.push_back(transaction);
    return;
  }
#endif
  if(_mem->addTransaction(false, addr)) {
    _read_queue.push_back(transaction);
    return;
  }
  assert(1==0); // Shouldnt reach here?
}

void SimObj::DRAM::read_complete(unsigned int id, uint64_t address, uint64_t clock_cycle) {
  for(auto it = _read_queue.begin(); it != _read_queue.end(); it++) {
    if(std::get<0>(*it) == address) {
      if(std::get<2>(*it) == true) {
        sequential_read_counter = 0;
        sequential_read_issued = false;
      }
      *(std::get<1>(*it)) = true;
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
      if(std::get<2>(*it) == true) {
        sequential_write_counter = 0;
        sequential_write_issued = false;
      }
      *(std::get<1>(*it)) = true;
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

void SimObj::DRAM::reset() {
  sequential_read_addr = SEQUENTIAL_START_ADDR;
  sequential_write_addr = SEQUENTIAL_START_ADDR;
}
