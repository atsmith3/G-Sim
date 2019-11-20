/*
 * Andrew Smith
 * 
 * DRAM Memory. Derived from Memory class, Uses DRAMSim2 to model the timing
 *
 */

#ifndef _DRAM_H
#define _DRAM_H

#include <list>
#include <tuple>
#include <vector>
#include "DRAMSim.h"
#include "memory.h"

namespace SimObj {

class DRAM : public Memory {
private:
  std::list<std::tuple<uint64_t, bool*, bool>> _write_queue;
  std::list<std::tuple<uint64_t, bool*, bool>> _read_queue;

  DRAMSim::TransactionCompleteCB *write_cb;
  DRAMSim::TransactionCompleteCB *read_cb;

  DRAMSim::MultiChannelMemorySystem *_mem;

  // DRAMSim2 Callbacks:
  void read_complete(unsigned int id, uint64_t address, uint64_t clock_cycle);
  void write_complete(unsigned int id, uint64_t address, uint64_t clock_cycle);

public:
  DRAM(void);
  DRAM(uint64_t access_latency, uint64_t write_latency, uint64_t num_simultaneous_requests);
  ~DRAM();

  void tick(void);
  void write(uint64_t addr, bool* complete, bool sequential=true);
  void read(uint64_t addr, bool* complete, bool sequential=true);

  void print_stats();
  void reset();
}; // class DRAM

} // namespace SimObj

#endif
