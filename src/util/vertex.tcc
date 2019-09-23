template<class vertex_t, class edge_t>
Vertex<vertex_t, edge_t>::Vertex(uint64_t vertex_id, vertex_t vertex_property) {
  this->vertex_property = vertex_property;
  this->certex_id = vertex_id;
}

template<class vertex_t, class edge_t>
Vertex<vertex_t, edge_t>::~Vertex() {
  // Do Nothing
}
