/*
 * Andrew Smith
 *
 * Read SRC Property:
 *  This is the frontend module of the graphicionado accelerator;
 *  it performs a sequential read on the value of the src vertex.
 *
 *  It has to interface with the DRAM memory to fetch the active
 *  property. Edges are stored in the Scratchpad.
 *
 */

#ifndef READSRCPROPERTY_H
#define READSRCPROPERTY_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>
#include <list>

#include "module.h"
#include "memory.h"

#include "readGraph.h"

namespace SimObj {

template<class v_t, class e_t>
class ReadSrcProperty : public Module<v_t, e_t> {
private:
  enum op_t {
    OP_WAIT,
    OP_MEM_WAIT,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_MEM_WAIT"},
    {2, "OP_NUM_OPS"}};
#endif

  using Module<v_t, e_t>::_tick;
  using Module<v_t, e_t>::_ready;
  using Module<v_t, e_t>::_stall;
  using Module<v_t, e_t>::_next;
  using Module<v_t, e_t>::_data;
  using Module<v_t, e_t>::_has_work;

  Memory* _dram;
  op_t _state;
  bool _fetched;
  std::list<uint64_t>* _process;
  Utility::readGraph<v_t>* _graph;

public:
  bool _mem_flag;

  uint64_t _vertex_id;
  ReadSrcProperty();
  ReadSrcProperty(Memory* dram, std::list<uint64_t>* process, Utility::readGraph<v_t>* graph);
  ~ReadSrcProperty();

  void tick(void);
};

} // namespace SimObj

#include "readSrcProperty.tcc"

#endif
