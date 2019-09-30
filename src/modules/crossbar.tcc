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
  _max_queue_size = 2;
  _num_ports = num_ports;
  _msg_queue.resize(num_ports);
  _in_module.resize(num_ports);
  _out_module.resize(num_ports);
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
bool SimObj::Crossbar<v_t, e_t>::send_data(Utility::pipeline_data<v_t, e_t> data) {
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

template<class v_t, class e_t>
bool SimObj::Crossbar<v_t, e_t>::has_data(uint64_t port_num) {
  assert(port_num < _num_ports);
  return !_msg_queue[port_num].empty();
}

template<class v_t, class e_t>
Utility::pipeline_data<v_t, e_t> SimObj::Crossbar<v_t, e_t>::get_data(uint64_t port_num) {
  assert(port_num < _num_ports);
  assert(!_msg_queue[port_num].empty());
  Utility::pipeline_data<v_t, e_t> ret = _msg_queue[port_num].front();
  _msg_queue[port_num].pop();
  return ret;
}

template<class v_t, class e_t>
void SimObj::Crossbar<v_t, e_t>::tick() {
  // Do Nothing
}
