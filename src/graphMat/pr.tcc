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
    message.delta = vertex.residual/(float)vertex.degree;
  }
}

template<class v_t, class e_t>
bool GraphMat::PR<v_t, e_t>::apply(const v_t& scratch, v_t& dram) {
  dram = alpha + (1.0-alpha)*scratch;
  return true;
}
