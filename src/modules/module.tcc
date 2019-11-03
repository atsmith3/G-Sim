/*
 * Andrew Smith
 *
 * Base Class for a Pipeline Module
 *
 */

#include "module.h"

template<class v_t, class e_t>
SimObj::Module<v_t, e_t>::Module() {
  _tick = 0;
  _stall = STALL_CAN_ACCEPT;
  _stall_count = 0;
  _stall_ticks.resize(STALL_NUM_TYPES, 0);
  _next = NULL;
  _prev = NULL;
  _has_work = false;
}


template<class v_t, class e_t>
SimObj::Module<v_t, e_t>::~Module() {
  // Do Nothing
}

template<class v_t, class e_t>
SimObj::stall_t SimObj::Module<v_t, e_t>::is_stalled(void) {
  return _stall;
}

template<class v_t, class e_t>
SimObj::stall_t SimObj::Module<v_t, e_t>::is_stalled(Utility::pipeline_data<v_t, e_t> data) {
  return _stall;
}

template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::tick(void) {
  _tick++;
}

template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::receive_message(SimObj::msg_t msg) {
  // Process Message
}

template<class v_t, class e_t>
uint64_t SimObj::Module<v_t, e_t>::get_attr(void) {
  // TODO: Return Vertex ID
  return 0;
}

template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::ready(void) {
  _ready = true;
  _has_work = true;
  _items_processed++;
}

template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::ready(Utility::pipeline_data<v_t, e_t> data) {
  // Does Nothing
  _data = data;
  _ready = true;
  _has_work = true;
  _items_processed++;
}

template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::set_next(SimObj::Module<v_t, e_t>* next) {
  _next = next;
}
  
template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::set_prev(SimObj::Module<v_t, e_t>* prev) {
  _prev = prev;
}

template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::update_stats() {
  _stall_ticks[_stall]++;
  if(_stall != STALL_CAN_ACCEPT) {
    _stall_count++;
  }
}

template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::set_name(std::string name) {
  _name = name;
}

template<class v_t, class e_t>
bool SimObj::Module<v_t, e_t>::busy() {
  return _has_work;
}

template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::print_stats() {
  std::cout << "-------------------------------------------------------------------------------\n";
  std::cout << "[ " << _name << " ]\n";
  std::cout << "  Stalls:\n";
  std::cout << "    STALL_CAN_ACCEPT: " << _stall_ticks[STALL_CAN_ACCEPT] << " cycles\n";
  std::cout << "    STALL_PROCESSING: " << _stall_ticks[STALL_PROCESSING] << " cycles\n";
  std::cout << "    STALL_PIPE:       " << _stall_ticks[STALL_PIPE] << " cycles\n";
  std::cout << "    STALL_MEM:        " << _stall_ticks[STALL_MEM] << " cycles\n";
  std::cout << "  Performance:\n";
  std::cout << "    Items Processed:  " << _items_processed << "\n";
}

template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::print_stats_csv() {
  std::cout << _name << "," << _stall_ticks[STALL_CAN_ACCEPT] << ","
    << _stall_ticks[STALL_PROCESSING] << ","
    << _stall_ticks[STALL_PIPE] << ","
    << _stall_ticks[STALL_MEM] << ","
    << _items_processed << "\n";
}

template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::clear_stats() {
  for(auto it = _stall_ticks.begin(); it != _stall_ticks.end(); it++) {
    *it = 0;
  }
  _stall_count = 0;
  _items_processed = 0;
}

#ifdef DEBUG
template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::set_stall(stall_t stall) {
  _stall = stall;
}
#endif
