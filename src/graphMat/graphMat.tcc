template<class v_t, class e_t>
void GraphMat::GraphApp<v_t, e_t>::initialize(Utility::Graph<v_t, e_t>& graph, std::list<uint64_t>* curr) {

}

template<class v_t, class e_t>
void GraphMat::GraphApp<v_t, e_t>::do_every_iteration(Utility::Graph<v_t, e_t>& graph, std::list<uint64_t>* curr) {

}

template<class v_t, class e_t>
bool GraphMat::GraphApp<v_t, e_t>::reduce(v_t& scratch, const v_t& message) {
  // Reduce
  return true;
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
