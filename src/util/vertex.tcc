template<class vertex_t, class edge_t>
Vertex<vertex_t, edge_t>::Vertex(uint64_t vertex_id, vertex_t vertex_property) {
  this->property = vertex_property;
  this->id = vertex_id;
  this->curr_edge = 0;
}

template<class vertex_t, class edge_t>
Vertex<vertex_t, edge_t>::~Vertex() {
  // Do Nothing
}
