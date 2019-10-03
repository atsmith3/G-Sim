template<class v_t, class e_t>
void GraphMat::GraphApp<v_t, e_t>::reduce(v_t& scratch, const v_t& message) {
  // Reduce
}

template<class v_t, class e_t>
void GraphMat::GraphApp<v_t, e_t>::process_edge(v_t& message, const e_t& edge, const v_t& vertex) {
  // Process Edge
}

template<class v_t, class e_t>
bool GraphMat::GraphApp<v_t, e_t>::apply(const v_t& scratch, v_t& dram) {
  // Apply
  return true;
}

template<class v_t, class e_t>
void GraphMat::GraphApp<v_t, e_t>::do_every_iteration() {
  iteration++;
}
