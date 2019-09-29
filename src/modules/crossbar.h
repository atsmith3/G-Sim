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

template<class v_t, class e_t>
class Crossbar : public Module<v_t, e_t> {
private:
  uint64_t _max_queue_size;
  uint64_t _num_ports;
  std::vector<std::queue<Utility::pipeline_data<v_t, e_t>>> _msg_queue;
  std::vector<Module<v_t, e_t>*> _in_module;
  std::vector<Module<v_t, e_t>*> _out_module;
  
  uint64_t route(Utility::pipeline_data<v_t, e_t> vertex);
  
public:
  Crossbar(uint64_t num_ports);
  ~Crossbar();

  void connect_input(Module<v_t, e_t>* in_module, uint64_t port_num);
  void connect_output(Module<v_t, e_t>* out_module, uint64_t port_num);

  // Input Side Interface:
  bool send_data(Utility::pipeline_data<v_t, e_t> data);

  // Output Side Interface:
  bool has_data(uint64_t port_num);
  Utility::pipeline_data<v_t, e_t> get_data(uint64_t port_num);

  void tick();
};

} // namespace SimObj

#include "crossbar.tcc"

#endif
