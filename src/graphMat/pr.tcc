template<class v_t, class e_t>
void GraphMat::PR<v_t, e_t>::initialize(Utility::Graph<v_t, e_t>& graph, std::list<uint64_t>* curr) {
  for(uint64_t i = 0; i < graph.vertex.size(); i++) {
    curr->push_back(i);
  }
}

template<class v_t, class e_t>
void GraphMat::PR<v_t, e_t>::do_every_iteration(Utility::Graph<v_t, e_t>& graph, std::list<uint64_t>* curr) {

}

template<class v_t, class e_t>
bool GraphMat::PR<v_t, e_t>::reduce(v_t& scratch, const v_t& message) {
  scratch.pageRank += message.residual;
  return true;
}

template<class v_t, class e_t>
void GraphMat::PR<v_t, e_t>::process_edge(v_t& message, const e_t& edge, const v_t& vertex) {
  if (vertex.degree == 0) {
    message.delta = 0.0;
  } else {
    message.delta = vertex.pagerank/(float)vertex.degree;
  }
}

template<class v_t, class e_t>
bool GraphMat::PR<v_t, e_t>::apply(const v_t& scratch, v_t& dram) {
  dram.pageRank = ALPHA + (1.0-ALPHA)*scratch.delta;
  return true;
}
