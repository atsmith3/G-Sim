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
  _logger = NULL;
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
#if MODULE_TRACE
  if(_logger != NULL) {
    _logger->write(std::to_string(_tick)+","+
                   std::to_string(_data.vertex_id_addr)+","+
                   std::to_string(_data.vertex_dst_id)+","+
                   std::to_string(_data.vertex_dst_id_addr)+","+
                   std::to_string(_data.edge_id)+","+
                   std::to_string(_data.vertex_data)+","+
                   std::to_string(_data.vertex_dst_data)+","+
                   std::to_string(_data.message_data)+","+
                   std::to_string(_data.vertex_temp_dst_data)+","+
                   std::to_string(_data.edge_data)+","+
                   std::to_string(_data.edge_temp_data)+","+
                   std::to_string(_data.updated)+"\n");
  }
#endif
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
  sim_out.write("-------------------------------------------------------------------------------\n");
  sim_out.write("[ " + _name + " ]\n");
  sim_out.write("  Stalls:\n");
  sim_out.write("    STALL_CAN_ACCEPT: " + std::to_string(_stall_ticks[STALL_CAN_ACCEPT]) + " cycles\n");
  sim_out.write("    STALL_PROCESSING: " + std::to_string(_stall_ticks[STALL_PROCESSING]) + " cycles\n");
  sim_out.write("    STALL_PIPE:       " + std::to_string(_stall_ticks[STALL_PIPE]) + " cycles\n");
  sim_out.write("    STALL_MEM:        " + std::to_string(_stall_ticks[STALL_MEM]) + " cycles\n");
  sim_out.write("  Performance:\n");
  sim_out.write("    Items Processed:  " + std::to_string(_items_processed) + "\n");
}

template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::print_stats_csv() {
  sim_out.write(_name + "," 
    + std::to_string(_stall_ticks[STALL_CAN_ACCEPT]) + ","
    + std::to_string(_stall_ticks[STALL_PROCESSING]) + ","
    + std::to_string(_stall_ticks[STALL_PIPE]) + ","
    + std::to_string(_stall_ticks[STALL_MEM]) + ","
    + std::to_string(_items_processed) + "\n");
}

template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::clear_stats() {
  for(auto it = _stall_ticks.begin(); it != _stall_ticks.end(); it++) {
    *it = 0;
  }
  _stall_count = 0;
  _items_processed = 0;
}

template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::reset(void) {
  // Do nothing
}

#ifdef DEBUG
template<class v_t, class e_t>
void SimObj::Module<v_t, e_t>::set_stall(stall_t stall) {
  _stall = stall;
}
#endif
