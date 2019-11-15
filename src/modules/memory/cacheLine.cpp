/*
 * Andrew Smith
 * 
 * 111319
 *
 */

#include <cassert>

#include "cacheLine.h"

SimObj::CacheLine::CacheLine() {
  address = 0x0;
  dirty = false;
  lru = 0;
}


SimObj::CacheLine::~CacheLine() {
  /* Do Nothing */
}


void SimObj::CacheLine::access(bool is_write) {
  if(is_write) dirty = true;
  lru = 0;
}


bool SimObj::CacheLine::hit(uint64_t addr) {
  return ((addr & ~ACCESS_MASK) == _address) && valid;
}


bool SimObj::CacheLine::valid() {
  return valid;
}


bool SimObj::CacheLine::dirty() {
  return dirty;
}


uint64_t SimObj::CacheLine::getAddr() {
  return addr;
}


uint64_t SimObj::CacheLine::getLRU() {
  return lru;
}


void SimObj::CacheLine::insert(uint64_t addr) {
  lru = 0;
  address = addr & ~ACCESS_MASK;
  valid = true;
  dirty = false;
}

void SimObj::CacheLine::evict() {
  valid = false;
}

void SimObj::CacheLine::tick() {
  lru++;
}
