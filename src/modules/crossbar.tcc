/*
 * Andrew Smith
 *
 * Crossbar Pipeline Module
 *
 */

#include <cassert>

template<class v_t, class e_t>
SimObj::Crossbar<v_t, e_t>::Crossbar(uint64_t num_ports) {
  assert(num_ports > 0);
  _max_queue_size = 1;
  _num_ports = num_ports;
  _msg_queue.resize(num_ports);
  _in_module.resize(num_ports);
  _out_module.resize(num_ports);
  _input_items.resize(num_ports);
  _output_items.resize(num_ports);
}

template<class v_t, class e_t>
SimObj::Crossbar<v_t, e_t>::~Crossbar() {
  // Do Nothing
}

template<class v_t, class e_t>
void SimObj::Crossbar<v_t, e_t>::connect_input(Module<v_t, e_t>* in_module, uint64_t port_num) {
  assert(port_num < _num_ports);
  assert(in_module != NULL);
  _in_module[port_num] = in_module;
}

template<class v_t, class e_t>
void SimObj::Crossbar<v_t, e_t>::connect_output(Module<v_t, e_t>* out_module, uint64_t port_num) {
  assert(port_num < _num_ports);
  assert(out_module != NULL);
  _out_module[port_num] = out_module;
}

template<class v_t, class e_t>
void SimObj::Crossbar<v_t, e_t>::ready(Utility::pipeline_data<v_t, e_t> data) {
  _msg_queue[route(data)].push(data);
  _output_items[route(data)]++;
}

template<class v_t, class e_t>
SimObj::stall_t SimObj::Crossbar<v_t, e_t>::is_stalled(Utility::pipeline_data<v_t, e_t> data) {
  if(_msg_queue[route(data)].size() < _max_queue_size) {
    return STALL_CAN_ACCEPT;
  }
  return STALL_PIPE;
}

template<class v_t, class e_t>
void SimObj::Crossbar<v_t, e_t>::tick() {
  /* Loop over message Queues, signal a module as ready if the queue !empty and
   * the corresponding pipeline stage can accept data. */
  for(uint64_t pipeline_id = 0; pipeline_id < _msg_queue.size(); pipeline_id++) {
    if(_out_module[pipeline_id]->is_stalled() == STALL_CAN_ACCEPT && !_msg_queue[pipeline_id].empty()) {
      Utility::pipeline_data<v_t, e_t> ret = _msg_queue[pipeline_id].front();
      _msg_queue[pipeline_id].pop();
      //std::cout << "Queue[" << pipeline_id << "] Size = " << _msg_queue[pipeline_id].size() << "\n";
      _out_module[pipeline_id]->ready(ret);
    }
  }
}

template<class v_t, class e_t>
bool SimObj::Crossbar<v_t, e_t>::busy() {
  for(auto it = _msg_queue.begin(); it != _msg_queue.end(); it++) {
    if(!it->empty()) {
      return true;
    }
  }
  return false;
}

template<class v_t, class e_t>
void SimObj::Crossbar<v_t, e_t>::clear_stats() {
  for(auto & element : _input_items) {
    element = 0;
  }
  for(auto & element : _output_items) {
    element = 0;
  }
  _items_processed = 0;
}

template<class v_t, class e_t>
void SimObj::Crossbar<v_t, e_t>::print_stats() {
  std::cout << "-------------------------------------------------------------------------------\n";
  std::cout << "[ " << _name << " ]\n";
  std::cout << "  Input Distribution:\n";
  std::cout << "    ";
  for(auto & element : _input_items) {
    std::cout << element << ", ";
  }
  std::cout << "\n";
  std::cout << "  Output Distribution:\n";
  std::cout << "    ";
  for(auto & element : _output_items) {
    std::cout << element << ", ";
  }
  std::cout << "\n";
  std::cout << "  Performance:\n";
  std::cout << "    Items Processed:  " << _items_processed << "\n";
}

template<class v_t, class e_t>
void SimObj::Crossbar<v_t, e_t>::print_stats_csv() {
  std::cout << _name << ",";
  std::cout << "input_distribution,";
  for(auto & element : _input_items) {
    std::cout << element << ",";
  }
  std::cout << "output_distribution,";
  for(auto & element : _output_items) {
    std::cout << element << ",";
  }
  std::cout << "performance," << _items_processed << "\n";
}
