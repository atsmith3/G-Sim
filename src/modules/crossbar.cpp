/*
 * Andrew Smith
 *
 * Crossbar Pipeline Module
 *
 */

#include <cassert>

#include "crossbar.h"

SimObj::Crossbar::Crossbar(uint64_t num_ports) {
  assert(num_ports > 0);
  _max_queue_size = 2;
  _num_ports = num_ports;
  _msg_queue.resize(num_ports);
  _in_module.resize(num_ports);
  _out_module.resize(num_ports);
}

SimObj::Crossbar::~Crossbar() {
  // Do Nothing
}

void SimObj::Crossbar::connect_input(Module* in_module, uint64_t port_num) {
  assert(port_num < _num_ports);
  assert(in_module != NULL);
  _in_module[port_num] = in_module;
}

void SimObj::Crossbar::connect_output(Module* out_module, uint64_t port_num) {
  assert(port_num < _num_ports);
  assert(out_module != NULL);
  _out_module[port_num] = out_module;
}

bool SimObj::Crossbar::send_data(uint64_t data) {
  if(_msg_queue[route(data)].size() >= _max_queue_size) {
    // Message cannot be delivered:
    return false;
  }
  else {
    // Message can be delivered:
    _msg_queue[route(data)].push(data);
    return true;
  }
  return false;
}

bool SimObj::Crossbar::has_data(uint64_t port_num) {
  assert(port_num < _num_ports);
  return !_msg_queue[port_num].empty();
}

uint64_t SimObj::Crossbar::get_data(uint64_t port_num) {
  assert(port_num < _num_ports);
  assert(!_msg_queue[port_num].empty());
  uint64_t ret = _msg_queue[port_num].front();
  _msg_queue[port_num].pop();
  return ret;
}

void SimObj::Crossbar::tick() {
  // Do Nothing
}

uint64_t SimObj::Crossbar::route(uint64_t vertex) {
  return vertex % _num_ports;
}
