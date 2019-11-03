/*
 * Andrew Smith
 *
 * Memory Base Class
 *
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <queue>
#include <vector>
#include "DRAMSim.h"

namespace SimObj {

enum mem_op_t {
  MEM_READ,
  MEM_WRITE,
  MEM_NUM_OPS
};

class MemRequest {
private:
  bool* _complete;
  uint64_t _start;
  uint64_t _delay;
  bool _active;
  mem_op_t _type;

public:
  MemRequest();
  MemRequest(bool* complete, uint64_t delay, mem_op_t type);
  ~MemRequest();

  void set_start(uint64_t start);
  void set_status(bool active);
  bool get_status(void);
  uint64_t get_finish_tick(void);
  void complete(void);
};

class Memory {
protected:
  uint64_t _tick;
  uint64_t _access_latency;
  uint64_t _write_latency;
  uint64_t _num_simultaneous_requests;
  std::vector<MemRequest> _action;
  std::queue<MemRequest> _req_queue;

public:
  Memory(void);
  Memory(uint64_t access_latency, uint64_t write_latency, uint64_t num_simultaneous_requests);
  virtual ~Memory();

  virtual void tick(void);
  virtual void write(uint64_t addr, bool* complete, bool sequential=true);
  virtual void read(uint64_t addr, bool* complete, bool sequential=true);
  virtual void print_stats();
  virtual void reset();
};

} // namespace SimObj
#endif
