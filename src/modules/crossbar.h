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
  
  uint64_t route(Utility::pipeline_data<v_t, e_t> vertex) {
    return vertex.vertex_dst_id % _num_ports;
  }

  // Stats
  using Module<v_t, e_t>::_items_processed;
  using Module<v_t, e_t>::_name;
  std::vector<uint64_t> _input_items;
  std::vector<uint64_t> _output_items;
  
public:



  Crossbar(uint64_t num_ports);
  ~Crossbar();

  void connect_input(Module<v_t, e_t>* in_module, uint64_t port_num);
  void connect_output(Module<v_t, e_t>* out_module, uint64_t port_num);

  stall_t is_stalled(Utility::pipeline_data<v_t, e_t> data);
  void ready(Utility::pipeline_data<v_t, e_t> data);
  bool busy();
  void clear_stats();
  void print_stats();
  void print_stats_csv();

  void tick();
};

} // namespace SimObj

#include "crossbar.tcc"

#endif
