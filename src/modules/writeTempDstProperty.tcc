/*
 * Andrew Smith
 *
 * WriteTempDstProperty
 *  This is the last stage in the processing phase, it writes the temp value
 *  back to memory and tells the ControlAtomicUpdate module a edge has finished.
 */

#include <cassert>

template<class v_t, class e_t>
SimObj::WriteTempDstProperty<v_t, e_t>::WriteTempDstProperty() {
  _scratchpad = NULL;
  _scratch_mem = NULL;
  _cau = NULL;
  _apply = NULL;
  _ready = false;
  _mem_flag = false;
  _complete = false;
  _state = OP_WAIT;
}


template<class v_t, class e_t>
SimObj::WriteTempDstProperty<v_t, e_t>::WriteTempDstProperty(Memory* scratchpad, ControlAtomicUpdate<v_t, e_t>* cau, std::map<uint64_t, Utility::pipeline_data<v_t, e_t>>* scratch_mem, std::list<uint64_t>* apply) {
  assert(scratchpad != NULL);
  assert(cau != NULL);
  assert(scratch_mem != NULL);
  assert(apply != NULL);
  _apply = apply;
  _scratchpad = scratchpad;
  _cau = cau;
  _scratch_mem = scratch_mem;
  _ready = false;
  _mem_flag = false;
  _complete = false;
  _state = OP_WAIT;
  _edges_written = 0;
}


template<class v_t, class e_t>
SimObj::WriteTempDstProperty<v_t, e_t>::~WriteTempDstProperty() {
  _scratchpad = NULL;
  _scratch_mem = NULL;
  _cau = NULL;
  _apply = NULL;
}


template<class v_t, class e_t>
void SimObj::WriteTempDstProperty<v_t, e_t>::tick(void) {
  _tick++;
  op_t next_state;

  // Module State Machine
  switch(_state) {
    case OP_WAIT : {
      if(_ready && !_complete) {
#ifdef MODULE_DEBUG
        std::cout << "Tick:" << _tick << " " << _name << " recieved: " << _data << "\n";
#endif
        _ready = false;
        _mem_flag = false;
        _scratchpad->write(0x01, &_mem_flag);
        _stall = STALL_MEM;
        next_state = OP_MEM_WAIT;
      }
      else {
        // Wait for upstream to send _edge
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
      }
      break;
    }
    case OP_MEM_WAIT : {
      if(_mem_flag) {
        _scratch_mem->insert_or_assign(_data.vertex_dst_id, _data);
        _apply->push_back(_data.vertex_dst_id);
        _edges_written++;
        _cau->receive_message(MSG_ATOMIC_OP_COMPLETE);
        next_state = OP_WAIT;
        _stall = STALL_CAN_ACCEPT;
        if(_data.last_vertex && _data.last_edge) {
          _complete = true;
        }
      }
      else {
        next_state = OP_MEM_WAIT;
        _stall = STALL_MEM;
      }
      break;
    }
    default : {

    }
  }
#if 0
  if(_state != next_state) {
    std::cout << "[ " << __PRETTY_FUNCTION__ << " ] tick: " << _tick << "  state: " << _state_name[_state] << "  next_state: " << _state_name[next_state] << "\n";
  }
#endif
  _state = next_state;
  this->update_stats();
}


template<class v_t, class e_t>
void SimObj::WriteTempDstProperty<v_t, e_t>::print_stats(void) {
  std::cout << "-------------------------------------------------------------------------------\n";
  std::cout << "[ " << _name << " ]\n";
  std::cout << "  Stalls:\n";
  std::cout << "    STALL_CAN_ACCEPT: " << _stall_ticks[STALL_CAN_ACCEPT] << " cycles\n";
  std::cout << "    STALL_PROCESSING: " << _stall_ticks[STALL_PROCESSING] << " cycles\n";
  std::cout << "    STALL_PIPE:       " << _stall_ticks[STALL_PIPE] << " cycles\n";
  std::cout << "    STALL_MEM:        " << _stall_ticks[STALL_MEM] << " cycles\n";
  std::cout << "  Performance:\n";
  std::cout << "    Edges:            " << _edges_written << "\n";
  std::cout << "    Cycles:           " << _tick << "\n";
}


template<class v_t, class e_t>
void SimObj::WriteTempDstProperty<v_t, e_t>::print_stats_csv(void) {
  std::cout << _name << "," << _stall_ticks[STALL_CAN_ACCEPT] << ","
    << _stall_ticks[STALL_PROCESSING] << ","
    << _stall_ticks[STALL_PIPE] << ","
    << _stall_ticks[STALL_MEM] << ","
    << _edges_written << ",\n";
}


template<class v_t, class e_t>
bool SimObj::WriteTempDstProperty<v_t, e_t>::complete() {
  return _complete;
}


template<class v_t, class e_t>
void SimObj::WriteTempDstProperty<v_t, e_t>::flush() {
  _complete = false;
}
