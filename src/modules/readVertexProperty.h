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

#include "module.h"
#include "memory.h"

namespace SimObj {


template<class v_t, class e_t>
class ReadVertexProperty : public Module<v_t, e_t> {
private:
  enum op_t {
    OP_WAIT,
    OP_MEM_WAIT,
    OP_SEND_DOWNSTREAM,
    OP_NUM_OPS
  };

#ifdef DEBUG
  std::map<int, std::string> _state_name = {
    {0, "OP_WAIT"},
    {1, "OP_MEM_WAIT"},
    {2, "OP_SEND_DOWNSTREAM"},
    {3, "OP_NUM_OPS"}};
#endif

  Memory* _dram;
  op_t _state;
  std::queue<uint64_t>* _apply;
  Utility::readGraph<v_t> _graph;

public:
  bool _mem_flag;
  ReadVertexProperty();
  ReadVertexProperty(Memory* dram, std::queue<uint64_t>* apply, Utility::readGraph<v_t>* graph);
  ~ReadVertexProperty();

  void tick(void);
  void ready(void);
};

} // namespace SimObj

#endif


