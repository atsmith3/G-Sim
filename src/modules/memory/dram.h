/*
 * Andrew Smith
 * 
 * DRAM Memory. Derived from Memory class, Uses DRAMSim2 to model the timing
 *
 */

#ifndef _DRAM_H
#define _DRAM_H

#include <queue>
#include <vector>
#include "DRAMSim.h"
#include "memory.h"

namespace SimObj {

class DRAM : public Memory {
private:
  std::vector<unsigned int> _action;
  std::queue<unsigned int> _req_queue;

  DRAMSim::MultiChannelMemorySystem *_mem;

public:
  DRAM(void);
  DRAM(uint64_t access_latency, uint64_t write_latency, uint64_t num_simultaneous_requests);
  ~DRAM();

  void tick(void);
  void write(uint64_t addr, bool* complete);
  void read(uint64_t addr, bool* complete);

  // DRAMSim2 Callbacks:
  void read_complete(unsigned int id, uint64_t address, uint64_t clock_cycle);
  void write_complete(unsigned int id, uint64_t address, uint64_t clock_cycle);
}; // class DRAM

} // namespace SimObj

#endif
