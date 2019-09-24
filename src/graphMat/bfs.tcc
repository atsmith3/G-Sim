template<class vertex_t, class edge_t>
void GraphMat::BFS<vertex_t, edge_t>::reduce(vertex_t& scratch, const vertex_t& message) {
  // Reduce
  scratch = message;
}

template<class vertex_t, class edge_t>
void GraphMat::BFS<vertex_t, edge_t>::process_edge(vertex_t& message, const edge_t& edge, const vertex_t& vertex) {
  // Process Edge
  message = vertex;
}

template<class vertex_t, class edge_t>
void GraphMat::BFS<vertex_t, edge_t>::apply(const vertex_t& scratch, vertex_t& dram) {
  // Apply
  dram = scratch;
}
