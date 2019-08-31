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
  _max_queue_size = 1;
  _num_ports = num_ports;
  _msg_queue.resize(num_ports);
}

SimObj::Crossbar::~Crossbar() {
  // Do Nothing
}

uint64_t SimObj::Crossbar::connect_input(Module* in_module) {

}

uint64_t SimObj::Crossbar::connect_output(Module* out_module) {

}

bool SimObj::Crossbar::send_data(uint64_t port_num, uint64_t data) {

}

SimObj::stall_t SimObj::Crossbar::is_stalled(uint64_t port_num) {

}

bool SimObj::Crossbar::has_data(uint64_t port_num) {

}

uint64_t SimObj::Crossbar::get_data(uint64_t port_num) {

}

void SimObj::Crossbar::tick() {

}

