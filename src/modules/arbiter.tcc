/*
 * Andrew Smith
 *
 * Arbiter
 *  Facilitates the arbitration in the parallel vertex reads.
 *
 */

#include <cassert>


template<class v_t, class e_t>
SimObj::Arbiter<v_t, e_t>::Arbiter() {
  _items_processed = 0;
}


template<class v_t, class e_t>
SimObj::Arbiter<v_t, e_t>::~Arbiter() {
  /* Do Nothing */
}


template<class v_t, class e_t>
void SimObj::Arbiter<v_t, e_t>::tick(void) {
  _tick++;
}


template<class v_t, class e_t>
SimObj::stall_t SimObj::Arbiter<v_t, e_t>::is_stalled(void) {
  return _next->is_stalled();
}


template<class v_t, class e_t>
void SimObj::Arbiter<v_t, e_t>::ready(Utility::pipeline_data<v_t, e_t> data) {
  _items_processed++;
  _next->ready(data);
}

template<class v_t, class e_t>
void SimObj::Arbiter<v_t, e_t>::print_stats() {
  sim_out.write("-------------------------------------------------------------------------------\n");
  sim_out.write("[ " + _name + " ]\n");
  sim_out.write("  Throughput:\n");
  sim_out.write("    workitems/tick: " + std::to_string((float)_items_processed/(float)_tick) + "\n");
  sim_out.write("    items Processed:  " + std::to_string(_items_processed) + "\n");
}

template<class v_t, class e_t>
void SimObj::Arbiter<v_t, e_t>::print_stats_csv() {
  sim_out.write(_name + "," 
    + std::to_string((float)_items_processed/(float)_tick) + ","
    + std::to_string(_items_processed) + "\n");
}

template<class v_t, class e_t>
void SimObj::Arbiter<v_t, e_t>::clear_stats() {
  _tick = 0;
  _items_processed = 0;
}
