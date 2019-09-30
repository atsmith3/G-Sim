/*
 * Andrew Smith
 *
 * Apply
 *  User Defined Function to Apply a Property to a vertex.
 *  min(in the case of SSSP)
 *
 */

#ifndef APPLY_H
#define APPLY_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>

#include "module.h"
#include "memory.h"

#include "graphApp.h"

namespace SimObj {

template<class v_t, class e_t>
class Apply : public Module<v_t, e_t> {
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
    {4, "OP_NUM_OPS"}};
#endif

  op_t _state;
  uint64_t _counter;
  bool _ready;
  uint64_t _delay_cycles;

public:
  Apply();
  Apply(int delay_cycles, GraphMat::GraphApp<v_t, e_t>* app);
  ~Apply();

  void tick(void);
};

} // namespace SimObj

#include "apply.tcc"

#endif // APPLY_H
