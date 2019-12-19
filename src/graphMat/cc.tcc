template<class v_t, class e_t>
void GraphMat::CC<v_t, e_t>::initialize(Utility::Graph<v_t, e_t>& graph, std::list<uint64_t>* curr) {
  uint64_t max = 0;
  uint64_t it = 0;
  for(uint64_t i = 0; i < graph.vertex.size(); i++) {
    if(graph.vertex[i].edges.size() > max) {
      max = graph.vertex[i].edges.size();
      it = i;
    }
  } 
  std::cout << "Starting @ vertex " << it << "\n";
  graph.vertex[it].property.component = 0;
  curr->push_back(it);
}

template<class v_t, class e_t>
void GraphMat::CC<v_t, e_t>::do_every_iteration(Utility::Graph<v_t, e_t>& graph, std::list<uint64_t>* curr) {

}

template<class v_t, class e_t>
bool GraphMat::CC<v_t, e_t>::reduce(v_t& scratch, const v_t& message) {
  // Reduce
  if(message < scratch) {
    scratch = message;
    return true;
  }
  return false;
}

template<class v_t, class e_t>
void GraphMat::CC<v_t, e_t>::process_edge(v_t& message, const e_t& edge, const v_t& vertex) {
  // Process Edge
  message = vertex;
}

template<class v_t, class e_t>
bool GraphMat::CC<v_t, e_t>::apply(const v_t& scratch, v_t& dram) {
  // Apply return true if successfully updated DRAM
  if(scratch != dram) {
    dram = scratch;
    return true;
  }
  return false;
}
