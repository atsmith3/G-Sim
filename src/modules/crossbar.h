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
  std::vector<Module*> _in_module;
  std::vector<Module*> _out_module;
  
  uint64_t route(uint64_t vertex);
  
public:
  Crossbar(uint64_t num_ports);
  ~Crossbar();

  void connect_input(Module* in_module, uint64_t port_num);
  void connect_output(Module* out_module, uint64_t port_num);

  // Input Side Interface:
  bool send_data(uint64_t data);

  // Output Side Interface:
  bool has_data(uint64_t port_num);
  uint64_t get_data(uint64_t port_num);

  void tick();
};

} // namespace SimObj

#endif
