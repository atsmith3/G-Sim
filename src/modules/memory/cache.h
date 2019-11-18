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

#include "cacheLine.h"
#include "prefetch.h"

namespace SimObj {

class Cache : public Memory {
private:
  struct mshr_t {
    uint64_t address;
    bool write;
    bool complete;
  };

  std::vector<cacheLine> cache;
  std::list<mshr_t*> mshr;

  std::list<std::pair<uint64_t, bool*>> outstanding_sequential_reads;

  Memory* _memory;

  int getLRU();

public:
  Cache();
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
