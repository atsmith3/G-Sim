/*
 * Andrew Smith
 *
 * Allocator
 *  Facilitates the arbitration in the parallel vertex reads.
 *
 */

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <iostream>
#include <cstdint>
#include <vector>

#include "module.h"

namespace SimObj {

template<class v_t, class e_t>
class Allocator : public Module<v_t, e_t> {
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
  using Module<v_t, e_t>::_data;
  using Module<v_t, e_t>::_name;
  using Module<v_t, e_t>::_has_work;

  std::vector<Module<v_t, e_t>*> _next;

  uint64_t _items_processed;

public:
  Allocator(std::vector<Module<v_t, e_t>*> next);
  ~Allocator();

  void print_stats();
  void print_stats_csv();
  void clear_stats();

  stall_t is_stalled(void);
  void ready(Utility::pipeline_data<v_t, e_t> data);

  void tick(void);
};

} // namespace SimObj

#include "allocator.tcc"

#endif // ALLOCATOR_H
