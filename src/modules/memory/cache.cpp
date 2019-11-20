/*
 *
 * Andrew Smith
 *
 * Really simple cache model. Used to buffer
 * the memory during the sequential accesses
 */

#include <cassert>
#include "cache.h"


SimObj::Cache::Cache(int entries, Memory* memory) {
  assert(memory != NULL);
  assert(entries > 0);

  cache.resize(entries);
  stats.resize(NUM_STATS, 0);

  _memory = memory;
}


SimObj::Cache::~Cache() {
  _memory = NULL;
}


uint64_t SimObj::Cache::getLRU() {
  uint64_t i = 0;
  uint64_t max = 0;
  uint64_t pos = 0;
  for(auto line : cache) {
    if(line.getLRU() > max) {
      max = line.getLRU();
      pos = i;
    }
    i++;
  }
  return pos;
}


uint64_t SimObj::Cache::hit(uint64_t addr) {
  uint64_t i = 0;
  for(auto line : cache) {
    if(line.hit(addr)) {
      return i;
    }
    i++;
  }
  return i;
}


void SimObj::Cache::tick(void) {
  // Search through the MSHR list and see if any outstanding requests completed
  // If it is a writeback remove and do nothing
  // If it is a read insert it into the cache
  _memory->tick();
  for(auto it : mshr) {
    if(it.complete) {
      if(!it.write) {
        uint64_t line = getLRU();
        if(cache[line].valid() && cache[line].dirty()) {
          stats[WRITEBACK]++;
          mshr_t req;
          req.address = cache[line].getAddr();
          req.write = true;
          req.complete = false;
          mshr.push_front(req);
          _memory->write(mshr.front().address, &mshr.front().complete, false);
        }
        cache[line].insert(it.address);
      }
    }
  }
  for(auto it = mshr.begin(); it != mshr.end(); it++) {
    if(it->complete) {
      it = mshr.erase(it);
    }
  }
  // Complete any outstanding requests:
  for(auto it = outstanding_sequential_reads.begin(); it != outstanding_sequential_reads.end(); it++) {
    if(hit(it->first) < cache.size()) {
      cache[hit(it->first)].access(false);
      *(it->second) = true;
      it = outstanding_sequential_reads.erase(it);
    }
  }
  // Tick all the cache lines
  for(auto & line : cache) {
    line.tick();
  }
}


void SimObj::Cache::write(uint64_t addr, bool* complete, bool sequential) {
  if(sequential) {
    stats[SEQ_WRITE]++;
    uint64_t line = hit(addr);
    if(line < cache.size()) {
      stats[HIT]++;
      cache[line].access(true);
      *complete = true;
    }
    else {
      stats[MISS]++;
      line = getLRU();
      if(cache[line].valid() && cache[line].dirty()) {
        stats[WRITEBACK]++;
        mshr_t req;
        req.address = cache[line].getAddr();
        req.write = true;
        req.complete = false;
        mshr.push_front(req);
        _memory->write(mshr.front().address, &mshr.front().complete, false);
      }
      cache[line].insert(addr);
      cache[line].access(true);
      *complete = true;
    }
  }
  else {
    stats[RAND_WRITE]++;
    _memory->write(addr, complete, false);
  }
}


/* 
 * SimObj::Cache::read
 *   If sequential Read, add req to MSHR if the line is not in the cache
 *   Else if in the cache access the line and set the req to complete
 *   If not sequential, submit a req to DRAM
 */
void SimObj::Cache::read(uint64_t addr, bool* complete, bool sequential) {
  if(sequential) {
    stats[SEQ_READ]++;
    uint64_t line = hit(addr);
    if(line < cache.size()) {
      stats[HIT]++;
      cache[line].access(false);
      *complete = true;
    }
    else {
      stats[MISS]++;
      mshr_t req;
      req.address = addr;
      req.write = false;
      req.complete = false;
      mshr.push_front(req);
      _memory->read(mshr.front().address, &mshr.front().complete, false);
      outstanding_sequential_reads.push_back(std::make_pair(addr, complete));
    }
  }
  else {
    stats[RAND_READ]++;
    _memory->read(addr, complete, false);
  }
}


void SimObj::Cache::print_stats() {
  SimObj::sim_out.write("Cache,");
  for(auto stat : stats) {
    SimObj::sim_out.write(std::to_string(stat)+",");
  }
  SimObj::sim_out.write("\n");
}


void SimObj::Cache::reset() {
  // Reset the stat counters:
  for(auto & stat : stats) stat = 0;
}


