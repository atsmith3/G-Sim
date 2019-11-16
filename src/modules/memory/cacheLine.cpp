/*
 * Andrew Smith
 * 
 * 111319
 *
 */

#include <cassert>

#include "cacheLine.h"

SimObj::CacheLine::CacheLine() {
  _address = 0x0;
  _dirty = false;
  _lru = 0;
  _valid = false;
}


SimObj::CacheLine::~CacheLine() {
  /* Do Nothing */
}


void SimObj::CacheLine::access(bool is_write) {
  if(is_write) _dirty = true;
  _lru = 0;
}


bool SimObj::CacheLine::hit(uint64_t addr) {
  return ((addr & ~ACCESS_MASK) == _address) && _valid;
}


bool SimObj::CacheLine::valid() {
  return _valid;
}


bool SimObj::CacheLine::dirty() {
  return _dirty;
}


uint64_t SimObj::CacheLine::getAddr() {
  return _address;
}


uint64_t SimObj::CacheLine::getLRU() {
  return _lru;
}


void SimObj::CacheLine::insert(uint64_t addr) {
  _lru = 0;
  _address = addr & ~ACCESS_MASK;
  _valid = true;
  _dirty = false;
}

void SimObj::CacheLine::evict() {
  _valid = false;
}

void SimObj::CacheLine::tick() {
  _lru++;
}
