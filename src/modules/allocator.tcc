/*
 * Andrew Smith
 *
 * Allocator
 *  Distributes workitems to available parallel vertex readers
 *
 */

#include <cassert>


template<class v_t, class e_t>
SimObj::Allocator<v_t, e_t>::Allocator(std::vector<Module<v_t, e_t>*> next) {
  _next = next;
  _items_processed = 0;
}


template<class v_t, class e_t>
SimObj::Allocator<v_t, e_t>::~Allocator() {
  /* Do Nothing */
}


template<class v_t, class e_t>
void SimObj::Allocator<v_t, e_t>::tick(void) {
  _tick++;
}


template<class v_t, class e_t>
SimObj::stall_t SimObj::Allocator<v_t, e_t>::is_stalled(void) {
  SimObj::stall_t temp = STALL_MEM;
  for(auto module = _next.begin(); module != _next.end(); module++) {
    if((*module)->is_stalled() == SimObj::STALL_CAN_ACCEPT) {
      temp = STALL_CAN_ACCEPT;
      return temp;
    }
  }
  return temp;
}


template<class v_t, class e_t>
void SimObj::Allocator<v_t, e_t>::ready(Utility::pipeline_data<v_t, e_t> data) {
  _items_processed++;
  for(auto module = _next.begin(); module != _next.end(); module++) {
    if((*module)->is_stalled() == SimObj::STALL_CAN_ACCEPT) {
      (*module)->ready(data);
      return;
    }
  }
  return;
}


template<class v_t, class e_t>
void SimObj::Allocator<v_t, e_t>::print_stats() {
  sim_out.write("-------------------------------------------------------------------------------\n");
  sim_out.write("[ " + _name + " ]\n");
  sim_out.write("  Throughput:\n");
  sim_out.write("    workitems/tick: " + std::to_string((float)_items_processed/(float)_tick) + " cycles\n");
  sim_out.write("    items Processed:  " + std::to_string(_items_processed) + "\n");
}

template<class v_t, class e_t>
void SimObj::Allocator<v_t, e_t>::print_stats_csv() {
  sim_out.write(_name + "," 
    + std::to_string((float)_items_processed/(float)_tick) + ","
    + std::to_string(_items_processed) + "\n");
}

template<class v_t, class e_t>
void SimObj::Allocator<v_t, e_t>::clear_stats() {
  _tick = 0;
  _items_processed = 0;
}
