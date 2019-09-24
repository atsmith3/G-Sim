/*
 *
 * Andrew Smith
 * 
 * Global frontier queue
 *
 */

#ifndef ATOMIC_QUEUE_H
#define ATOMIC_QUEUE_H

#include <queue>
#include <pthread.h>

namespace Utility {

template<class T>
class AtomicQueue {
private:
  std::queue<T> _queue;
  pthread_mutex_t _queue_mutex;

public:
  // Constructor
  AtomicQueue() { _queue_mutex = PTHREAD_MUTEX_INITIALIZER; }

  // Destructor
  ~AtomicQueue() { /* Do Nothing */ }

  // Push
  void push(const T obj);

  // Pop
  T pop();

  // Size
  uint64_t size();

  // IsEmpty
  bool isEmpty();

}; // class AtomicQueue

}; // namespace Utility

#include "atomicQueue.tcc"

#endif // ATOMIC_QUEUE_H
