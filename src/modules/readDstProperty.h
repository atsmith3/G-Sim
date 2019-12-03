/*
 * Andrew Smith
 *
 * Read DST Property:
 *  Read the destination property from DRAM. Optional depending on the 
 *  graph algorithm
 *
 */

#ifndef READDSTPROPERTY_H
#define READDSTPROPERTY_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>

#include "module.h"
#include "memory.h"

#include "readGraph.h"

namespace SimObj {

template<class v_t, class e_t>
class ReadDstProperty : public Module<v_t, e_t> {
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
  using Module<v_t, e_t>::_name;
  using Module<v_t, e_t>::_has_work;
#if MODULE_TRACE
  using Module<v_t, e_t>::_logger;
  bool mem_req_logged;
#endif

  Memory* _dram;
  op_t _state;
  Utility::readGraph<v_t>* _graph;
  uint64_t _id;
  uint64_t _rid;

public:
  bool _mem_flag;
  ReadDstProperty(Memory* dram, Utility::readGraph<v_t>* graph, std::string name, uint64_t id, uint64_t reader_id);
  ~ReadDstProperty();

  void tick(void);
};

} // namespace SimObj

#include "readDstProperty.tcc"

#endif
