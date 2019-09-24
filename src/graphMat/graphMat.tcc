template<class vertex_t, class edge_t>
void GraphMat::GraphApp<vertex_t, edge_t>::reduce(vertex_t& scratch, const vertex_t& message) {
  // Reduce
}

template<class vertex_t, class edge_t>
void GraphMat::GraphApp<vertex_t, edge_t>::process_edge(vertex_t& message, const edge_t& edge, const vertex_t& vertex) {
  // Process Edge
}

template<class vertex_t, class edge_t>
void GraphMat::GraphApp<vertex_t, edge_t>::apply(const vertex_t& scratch, vertex_t& dram) {
  // Apply
}

template<class vertex_t, class edge_t>
void GraphMat::GraphApp<vertex_t, edge_t>::do_every_iteration() {
  iteration++;
}
