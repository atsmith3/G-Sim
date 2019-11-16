/*
 *
 * Andrew Smith
 *
 * Really simple cache model. Used to buffer
 * the memory during the sequential accesses
 */

#include <cassert.h>
#include "cache.h"


SimObj::Cache::Cache(int entries, Memory* memory) {
  assert(memory != NULL);
  assert(entries > 0);

  cache.resize(entries);
  _memory = memory;
}


SimObj::Cache::~Cache() {
  _memory = NULL;
}


int SimObj::Cache::getLRU() {
  int i = 0;
  int max = 0;
  int pos = 0;
  for(auto line : cache) {
    if(line->getLRU() > max) {
      max = line->getLRU();
      pos = i;
    }
    i++;
  }
  return pos;
}


void SimObj::Cache::tick(void) {
  // Search through the MSHR list and see if any outstanding requests completed
  // If it is a writeback remove and do nothing
  // If it is a read insert it into the cache
  for(auto it : mshr) {
    if(it->complete) {
      if(it->read) {
        int line = getLRU();
        if(cache[line].valid() && cache[line].dirty()) {
          // Writeback

        }
        cache[pos].insert(it->address);
      }
      mshr.remove(it);
    }
  }
  // Tick all the cache lines
  for(auto line : cache) {
    line->tick();
  }
}


void SimObj::Cache::write(uint64_t addr, bool* complete, bool sequential=true) {

}


void SimObj::Cache::read(uint64_t addr, bool* complete, bool sequential=true) {

}


void SimObj::Cache::print_stats() {

}


void SimObj::Cache::reset() {

}


