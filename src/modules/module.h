/*
 * Andrew Smith
 *
 * Base Class for a Pipeline Module
 *
 */

#ifndef MODULE_H
#define MODULE_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

#include "pipeline_data.h"

namespace SimObj {

enum stall_t {
  STALL_CAN_ACCEPT,
  STALL_PROCESSING,
  STALL_PIPE,
  STALL_MEM,
  STALL_ATOMIC,
  STALL_NUM_TYPES
};

enum msg_t {
  MSG_ATOMIC_OP_COMPLETE,
  MSG_NUM_TYPES
};

template<class v_t, class e_t>
class Module {
protected:
  std::string _name;
  uint64_t _tick;
  stall_t _stall;
  uint64_t _stall_count;
  std::vector<uint64_t> _stall_ticks;
  
  // Neighbors
  Module* _next;
  Module* _prev;

  Utility::pipeline_data<v_t, e_t> _data;
  bool _ready;
  bool _has_work;     // Flag for determine if there is still work left in the pipeline stages

public:
  Module();
  virtual ~Module();

  virtual stall_t is_stalled(void);
  virtual stall_t is_stalled(Utility::pipeline_data<v_t, e_t> data);
  virtual void tick(void);
  virtual void receive_message(msg_t msg);
  uint64_t get_attr(void); 
  virtual void ready(void);
  virtual void ready(Utility::pipeline_data<v_t, e_t> data);
  void set_next(Module* next);
  void set_prev(Module* prev);
  virtual void update_stats();
  virtual void print_stats();
  virtual void print_stats_csv();
  virtual void set_name(std::string name);
  virtual bool busy();

#ifdef DEBUG
  void set_stall(stall_t stall);
#endif
};

} // namespace SimObj

#include "module.tcc"

#endif // MODULE_H
