/*
 * Andrew Smith
 *
 * Arbiter
 *  Facilitates the arbitration in the parallel vertex reads.
 *
 */

#ifndef ARBITER_H
#define ARBITER_H

#include <iostream>
#include <cstdint>

#include "module.h"

namespace SimObj {

template<class v_t, class e_t>
class Arbiter : public Module<v_t, e_t> {
private:
  enum op_t {
    OP_WAIT,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_NUM_OPS"}};
#endif

  using Module<v_t, e_t>::_tick;
  using Module<v_t, e_t>::_ready;
  using Module<v_t, e_t>::_stall;
  using Module<v_t, e_t>::_next;
  using Module<v_t, e_t>::_data;
  using Module<v_t, e_t>::_name;
  using Module<v_t, e_t>::_has_work;

  uint64_t _items_processed;

public:
  Arbiter();
  ~Arbiter();

  void print_stats();
  void print_stats_csv();
  void clear_stats();

  stall_t is_stalled(void);
  void ready(Utility::pipeline_data<v_t, e_t> data);

  void tick(void);
};

} // namespace SimObj

#include "arbiter.tcc"

#endif // ARBITER_H
