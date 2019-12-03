/*
 * Andrew Smith
 *
 * Read Vertex Property:
 *  Read the vertex property from DRAM.
 *
 */

#ifndef READVERTEXPROPERTY_H
#define READVERTEXPROPERTY_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <map>
#include <list>

#include "module.h"
#include "memory.h"

namespace SimObj {


template<class v_t, class e_t>
class ReadVertexProperty : public Module<v_t, e_t> {
private:
  enum op_t {
    OP_WAIT,
    OP_MEM_WAIT,
    OP_NUM_OPS
  };

  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_MEM_WAIT"},
    {2, "OP_NUM_OPS"}};
  
  using Module<v_t, e_t>::_tick;
  using Module<v_t, e_t>::_ready;
  using Module<v_t, e_t>::_data;
  using Module<v_t, e_t>::_stall;
  using Module<v_t, e_t>::_name;
  using Module<v_t, e_t>::_next;
  using Module<v_t, e_t>::_has_work;
  using Module<v_t, e_t>::_items_processed;
#if MODULE_TRACE
  using Module<v_t, e_t>::_logger;
  bool mem_req_logged;
#endif

  uint64_t _id;
  Memory* _dram;
  op_t _state;
  std::list<uint64_t>* _apply;
  Utility::readGraph<v_t>* _graph;
  uint64_t _base_addr;
  uint64_t _curr_addr;

public:
  bool _mem_flag;
  ReadVertexProperty(Memory* dram, std::list<uint64_t>* apply, Utility::readGraph<v_t>* graph, uint64_t base_addr, std::string name, uint64_t id);
  ~ReadVertexProperty();

  void tick(void);
  void reset(void);
};

} // namespace SimObj

#include "readVertexProperty.tcc"

#endif


