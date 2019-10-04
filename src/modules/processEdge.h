/*
 * Andrew Smith
 *
 * User Definable Function to process the edge.
 *  Used in certain applications.
 *  Purely computational delay
 *
 */

#ifndef PROCESSEDGE_H
#define PROCESSEDGE_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>

#include "graphMat.h"

#include "module.h"

namespace SimObj {

template<class v_t, class e_t>
class ProcessEdge : public Module<v_t, e_t> {
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
  using Module<v_t, e_t>::_name;

  op_t _state;
  uint64_t _counter;
  uint64_t _delay_cycles;
  GraphMat::GraphApp<v_t, e_t>* _graph_app;

public:
  ProcessEdge();
  ProcessEdge(int delay_cycles, GraphMat::GraphApp<v_t, e_t>* graph_app);
  ~ProcessEdge();

  void tick(void);
};

} // namespace SimObj

#include "processEdge.tcc"

#endif
