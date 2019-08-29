#include <iostream>
#include <vector>

#include "module.h"

int main() {
  SimObj::Module start;
  SimObj::Module middle;
  SimObj::Module end;

  start.set_prev(NULL);
  start.set_next(&middle);
  middle.set_prev(&start);
  middle.set_next(&end);
  end.set_prev(&middle);
  end.set_next(NULL);

  for(int i = 0; i < 10000; i++) {
    start.tick();
    middle.tick();
    end.tick();
  }
}
