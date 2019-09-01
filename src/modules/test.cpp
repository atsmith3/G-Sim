#include <iostream>
#include <vector>

#include "module.h"
#include "memory.h"
#include "crossbar.h"

int main() {
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
}
