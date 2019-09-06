#include <iostream>
#include <vector>

#include "module.h"
#include "memory.h"
#include "crossbar.h"
#include "readSrcProperty.h"
#include "readSrcEdges.h"
#include "readDstProperty.h"

int main() {
#if 0
  SimObj::Module start;
  SimObj::Module middle;
  SimObj::Module end;
  SimObj::Memory mem(10, 100, 2);

  start.set_prev(NULL);
  start.set_next(&middle);
  middle.set_prev(&start);
  middle.set_next(&end);
  end.set_prev(&middle);
  end.set_next(NULL);

  bool* mem_test = new bool[10];

  // Test Memory Class: Make Requests
  for(int i = 0; i < 10; i++) {
    mem_test[i] = false;
    if(i % 3 == 0) {
      mem.read(0x100*i, &mem_test[i]);
    }
    else {
      mem.write(0x100*i, &mem_test[i]);
    }
  }

  // Simulate
  for(int i = 0; i < 10000; i++) {
    start.tick();
    middle.tick();
    end.tick();
    mem.tick();

    // Check when memory accesses complete:
    for(int j = 0; j < 10; j++) {
      if(mem_test[j] == true) {
        mem_test[j] = false;
        std::cout << "MemTest " << j << " has finished @ tick: " << i << "\n";
      }
    }
  }

  // Crossbar Tests:
  SimObj::Crossbar xbar(3);
  for(int i = 0; i < 10; i++) {
    uint64_t data = rand() % 10;
    if(xbar.send_data(data)) {
      std::cout << "Sent " << data << "\n";
    }
    else {
      std::cout << "Failed to send " << data << "\n";
      std::cout << "Reading data from ports:\n";
      for(int j = 0; j < 10; j++) {
        if(xbar.has_data(j % 3)) {
          std::cout << "Read " << xbar.get_data(j % 3) << " From port " << j % 3 << "\n";
        }
      }
    }
  }
#endif

#if 0
  // Test readSrcProperty;
  SimObj::Module dummy;
  SimObj::Memory mem(10, 100, 2);
  SimObj::ReadSrcProperty p1(&mem);

  // Connect:
  p1.set_next(&dummy);
  p1.set_prev(NULL);
  dummy.set_next(NULL);
  dummy.set_prev(&p1);
  
  dummy.set_stall(SimObj::STALL_CAN_ACCEPT);

  for(int i = 0; i < 500; i++) {
    p1.tick();
    dummy.tick();
    mem.tick();

    if(i == 200) {
      dummy.set_stall(SimObj::STALL_PIPE);
    }
    if(i == 300) {
      dummy.set_stall(SimObj::STALL_CAN_ACCEPT);
    }
  }
#endif

#if 0
  // Test readSrcEdges;
  SimObj::Module dummy;
  SimObj::Memory mem(10, 100, 2);
  SimObj::Memory scratchpad(2, 10, 1);
  SimObj::ReadSrcProperty p1(&mem);
  SimObj::ReadSrcEdges p2(&scratchpad);

  // Connect:
  p1.set_next(&p2);
  p1.set_prev(NULL);
  p2.set_next(&dummy);
  p2.set_prev(&p1);
  dummy.set_next(NULL);
  dummy.set_prev(&p2);
  
  dummy.set_stall(SimObj::STALL_CAN_ACCEPT);

  for(int i = 0; i < 500; i++) {
    mem.tick();
    scratchpad.tick();
    p1.tick();
    p2.tick();
    dummy.tick();
  }
#endif

#if 1
  // Test readSrcEdges;
  SimObj::Module dummy;
  SimObj::Memory mem(10, 100, 2);
  SimObj::Memory scratchpad(2, 10, 1);
  SimObj::ReadSrcProperty p1(&mem);
  SimObj::ReadSrcEdges p2(&scratchpad);
  SimObj::ReadDstProperty p3(&mem);

  // Connect:
  p1.set_next(&p2);
  p1.set_prev(NULL);
  p2.set_next(&p3);
  p2.set_prev(&p1);
  p3.set_next(&dummy);
  p3.set_prev(&p2);
  dummy.set_next(NULL);
  dummy.set_prev(&p2);
  
  dummy.set_stall(SimObj::STALL_CAN_ACCEPT);

  for(int i = 0; i < 500; i++) {
    mem.tick();
    scratchpad.tick();
    p1.tick();
    p2.tick();
    p3.tick();
    dummy.tick();
  }
#endif
}
