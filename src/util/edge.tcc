template<class edge_t>
Edge<edge_t>::Edge(uint64_t src, uint64_t dst, edge_t prop) {
  this->src = src;
  this->dst = dst;
  this->edge_property = prop;
}

template<class edge_t>
Edge<edge_t>::~Edge() {
  // Do Nothing
}

template<class edge_t>
edge_t Edge<edge_t>::get_property() const {
  return edge_property;
}

template<class edge_t>
uint64_t Edge<edge_t>::get_src() const {
  return src;
}

template<class edge_t>
uint64_t Edge<edge_t>::get_dst() const {
  return dst;
}

template<class edge_t>
void Edge<edge_t>::set_property(edge_t property) {
  this->edge_property = property;
}
