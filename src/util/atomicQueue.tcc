template<class T>
void Utility::AtomicQueue<T>::push(const T obj) {
  pthread_mutex_lock(&this->_queue_mutex);
  _queue.push(obj);
  pthread_mutex_unlock(&this->_queue_mutex);
}


