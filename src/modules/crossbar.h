/*
 * Andrew Smith
 *
 * Crossbar Pipeline Module
 *
 */

#ifndef CROSSBAR_H
#define CROSSBAR_H

#include <iostream>
#include <vector>
#include <queue>

#include "module.h"

namespace SimObj {

class Crossbar : public Module {
private:
  uint64_t _max_queue_size;
  uint64_t _num_ports;
  std::vector<std::queue<uint64_t>> _msg_queue;
  
  uint64_t route(uint64_t vertex);
  
public:
  Crossbar(uint64_t num_ports);
  ~Crossbar();

  uint64_t connect_input(Module* in_module);
  uint64_t connect_output(Module* out_module);

  // Input Side Interface:
  bool send_data(uint64_t port_num, uint64_t data);
  stall_t is_stalled(uint64_t port_num);


  // Output Side Interface:
  bool has_data(uint64_t port_num);
  uint64_t get_data(uint64_t port_num);

  void tick();
};

} // namespace SimObj

#endif
