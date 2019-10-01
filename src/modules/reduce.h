/*
 * Andrew Smith
 *
 * Reduction Function
 * 
 * User Programmable to perform graph application task.
 *  Hardcoded computational delay for now,
 *  ToDo: Actually perform graph operation.
 *
 */


#ifndef REDUCE_H
#define REDUCE_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>

#include "module.h"

#include "graphMat.h"

namespace SimObj {

template<class v_t, class e_t>
class Reduce : public Module<v_t, e_t> {
private:
  enum op_t {
    OP_WAIT,
    OP_COUNT,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_COUNT"},
    {2, "OP_NUM_OPS"}};
#endif

  using Module<v_t, e_t>::_tick;
  using Module<v_t, e_t>::_ready;
  using Module<v_t, e_t>::_stall;
  using Module<v_t, e_t>::_next;
  using Module<v_t, e_t>::_data;

  op_t _state;
  uint64_t _counter;
  uint64_t _delay_cycles;
  GraphMat::GraphApp<v_t, e_t>* _app;

public:
  Reduce();
  Reduce(int delay_cycles, GraphMat::GraphApp<v_t, e_t>* app);
  ~Reduce();

  void tick(void);
};

} // namespace SimObj

#include "reduce.tcc"

#endif // REDUCE_H
