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

int SimObj::Cache::hit(uint64_t addr) {
  int i = 0;
  for(auto line : cache) {
    if(line->hit(addr)) {
      return i;
    }
    i++;
  }
}

void SimObj::Cache::tick(void) {
  // Search through the MSHR list and see if any outstanding requests completed
  // If it is a writeback remove and do nothing
  // If it is a read insert it into the cache
  for(auto it : mshr) {
    if((*it)->complete) {
      if((*it)->read) {
        int line = getLRU();
        if(cache[line].valid() && cache[line].dirty()) {
          mshr_t* req = new mshr_t;
          req->address = cache[line].getAddress();
          req->false = true;
          req->complete = false;
          mshr.push_back(req);
          _memory->write(req->address, &req->complete, false);
        }
        cache[line].insert(it->address);
      }
      delete *it;
      mshr.remove(it);
    }
  }
  // Complete any outstanding requests:
  for(auto it : outstanding_sequential_reads) {
    if(hit((*it)->first) < cache.size()) {
      cache[hit((*it)->first)].access(false);
      *((*it)->second) = true;
      outstanding_sequential_reads.remove(it);
    }
  }
  // Tick all the cache lines
  for(auto line : cache) {
    line->tick();
  }
}


void SimObj::Cache::write(uint64_t addr, bool* complete, bool sequential=true) {
  if(sequential) {
    int line = hit(addr);
    if(line < cache.size()) {
      cache[line].access(true);
      *complete = true;
    }
    else {
      line = getLRU();
      if(cache[line].valid() && cache[line].dirty()) {
        mshr_t* req = new mshr_t;
        req->address = cache[line].getAddress();
        req->false = true;
        req->complete = false;
        mshr.push_back(req);
        _memory->write(req->address, &req->complete, false);
      }
      cache[line].insert(addr);
      cache[line].access(true);
      *complete = true;
    }
  }
  else {
    _memory->write(addr, complete, false);
  }
}


void SimObj::Cache::read(uint64_t addr, bool* complete, bool sequential=true) {
  // If sequential Read, add req to MSHR if the line is not in the cache
  // Else if in the cache access the line and set the req to complete
  // If not sequential, submit a req to DRAM
  if(sequential) {
    int line = hit(addr);
    if(line < cache.size()) {
      cache[line].access(false);
      *complete = true;
    }
    else {
      line = getLRU();
      if(cache[line].valid() && cache[line].dirty()) {
        mshr_t* req = new mshr_t;
        req->address = cache[line].getAddress();
        req->false = true;
        req->complete = false;
        mshr.push_back(req);
        _memory->write(req->address, &req->complete, false);
      }
      cache[line].insert(addr);
      cache[line].access(true);
      *complete = true;
    }
  }
  else {
    _memory->write(addr, complete, false);
  }
}


void SimObj::Cache::print_stats() {

}


void SimObj::Cache::reset() {

}


