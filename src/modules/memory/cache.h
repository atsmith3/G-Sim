/*
 *
 * Andrew Smith
 * 
 * Really Simple Fully Associative Cache Model
 *   Used for sequential reads to reduce memory requests.
 *
 */

#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <list>
#include <vector>
#include <map>

#include "cacheLine.h"
#include "prefetch.h"
#include "log.h"

namespace SimObj {

class Cache : public Memory {
private:
  struct mshr_t {
    uint64_t address;
    bool write;
    bool complete;
    bool prefetch;
  };

  struct prefetch_t {
    uint64_t address;
    bool issued;
    bool complete;
  };

  std::vector<CacheLine> cache;
  std::list<mshr_t> mshr;

  std::list<std::pair<uint64_t, bool*>> outstanding_sequential_reads;
  // Address, Accessed this Cycle, Prefetch Issued
  std::map<uint64_t, std::pair<bool, bool>> prefetched;

  Memory* _memory;

  //Stats:
  enum stat_t {
    HIT,
    MISS,
    SEQ_READ,
    SEQ_WRITE,
    RAND_READ,
    RAND_WRITE,
    PREFETCH,
    WRITEBACK,
    NUM_STATS
  };
  std::vector<uint64_t> stats;

  uint64_t getLRU();
  uint64_t hit(uint64_t addr);

public:
  Cache(int entries, Memory* memory);
  ~Cache();
  
  void tick(void);
  void write(uint64_t addr, bool* complete, bool sequential=true);
  void read(uint64_t addr, bool* complete, bool sequential=true);

  void print_stats();
  void reset();
}; // class Cache

}; // namespace SimObj

#endif // CACHE_H
