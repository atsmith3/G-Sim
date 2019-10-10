#include <cassert>

template<class v_t, class e_t>
SimObj::Pipeline<v_t, e_t>::Pipeline(uint64_t pipeline_id, const Utility::Options opt, Utility::readGraph<v_t>* graph, GraphMat::GraphApp<v_t, e_t>* application, Memory* mem, Crossbar<v_t, e_t>* crossbar) {
  // Assert inputs are OK
  assert(graph != NULL);
  assert(applicaton != NULL);
  assert(mem != NULL);
  assert(crossbar != NULL);

  // Allocate Scratchpad
  scratchpad_map = new std::map<uint64_t, Utility::pipeline_data<v_t, e_t>>;
  SimObj::Memory* scratchpad = new SimObj::Memory(opt.scratchpad_read_latency, opt.scratchpad_write_latency, opt.scratchpad_num_simultaneous_requests)

  // Allocate apply queue
  apply = new std::list<uint64_t>;

  // Allocate Pipeline Modules
  p1 = new SimObj::ReadSrcProperty<vertex_t, edge_t>(mem, process, graph);
  p2 = new SimObj::ReadSrcEdges<vertex_t, edge_t>(scratchpad, graph);
  p3 = new SimObj::ReadDstProperty<vertex_t, edge_t>(mem, graph);
  p4 = new SimObj::ProcessEdge<vertex_t, edge_t>(1, application);
  p5 = new SimObj::ControlAtomicUpdate<vertex_t, edge_t>;
  p6 = new SimObj::ReadTempDstProperty<vertex_t, edge_t>(scratchpad, graph, scratchpad_map);
  p7 = new SimObj::Reduce<vertex_t, edge_t>(1, application);
  p8 = new SimObj::WriteTempDstProperty<vertex_t, edge_t>(scratchpad, p5, scratchpad_map, apply);

  a1 = new SimObj::ReadVertexProperty<vertex_t, edge_t>(mem, apply, graph);
  a2 = new SimObj::ReadTempVertexProperty<vertex_t, edge_t>(scratchpad, graph, scratchpad_map);
  a3 = new SimObj::Apply<vertex_t, edge_t>(1, bfs);
  a4 = new SimObj::WriteVertexProperty<vertex_t, edge_t>(mem, process, graph);
  
  // Connect Pipeline
  p1->set_next(&p2);
  p1->set_prev(NULL);
  p2->set_next(&p3);
  p2->set_prev(&p1);
  p3->set_next(&p4);
  p3->set_prev(&p2);
  p4->set_next(&p5);
  p4->set_prev(&p3);
  p5->set_next(&p6);
  p5->set_prev(&p4);
  p6->set_next(&p7);
  p6->set_prev(&p5);
  p7->set_next(&p8);
  p7->set_prev(&p6);
  p8->set_next(NULL);
  p8->set_prev(&p7);

  a1->set_prev(NULL);
  a1->set_next(&a2);
  a2->set_prev(&a1);
  a2->set_next(&a3);
  a3->set_prev(&a2);
  a3->set_next(&a4);
  a4->set_prev(&a3);
  a4->set_next(NULL);

  // Name Modules
  p1->set_name("ReadSrcProperty " + std::to_string(pipeline_id));
  p2->set_name("ReadSrcEdges " + std::to_string(pipeline_id));
  p3->set_name("ReadDstProperty " + std::to_string(pipeline_id));
  p4->set_name("ProcessEdge " + std::to_string(pipeline_id));
  p5->set_name("ControlAtomicUpdate " + std::to_string(pipeline_id));
  p6->set_name("ReadTempDstProperty " + std::to_string(pipeline_id));
  p7->set_name("Reduce " + std::to_string(pipeline_id));
  p8->set_name("WriteTempDstProperty " + std::to_string(pipeline_id));

  a1->set_name("ReadVertexProperty " + std::to_string(pipeline_id));
  a2->set_name("ReadTempVertexProperty " + std::to_string(pipeline_id));
  a3->set_name("Apply " + std::to_string(pipeline_id));
  a4->set_name("WriteVertexProperty " + std::to_string(pipeline_id));
}

template<class v_t, class e_t>
SimObj::Pipeline<v_t, e_t>::~Pipeline() {
  delete scratchpad_map;
  scratchpad_map = NULL;
  delete scratchpad;
  scratchpad = NULL;
  delete apply;
  apply = NULL;

  delete p1;
  p1 = NULL;
  delete p2;
  p2 = NULL;
  delete p3;
  p3 = NULL;
  delete p4;
  p4 = NULL;
  delete p5;
  p5 = NULL;
  delete p6;
  p6 = NULL;
  delete p7;
  p7 = NULL;
  delete p8;
  p8 = NULL;

  delete a1;
  a1 = NULL;
  delete a2;
  a2 = NULL;
  delete a3;
  a3 = NULL;
  delete a4;
  a4 = NULL;

  process = NULL;
  crossbar = NULL;
}

template<class v_t, class e_t>
void SimObj::Pipeline<v_t, e_t>::tick_process() {
  _tick++;
  p1->tick();
  p2->tick();
  p3->tick();
  p4->tick();
  p5->tick();
  p6->tick();
  p7->tick();
  p8->tick();
  scratchpad->tick();
}

template<class v_t, class e_t>
void SimObj::Pipeline<v_t, e_t>::tick_apply() {
  _tick++;
  a1->tick();
  a2->tick();
  a3->tick();
  a4->tick();
  scratchpad->tick();
}

template<class v_t, class e_t>
bool SimObj::Pipeline<v_t, e_t>::process_complete() {
  return p8->complete();
}

template<class v_t, class e_t>
bool SimObj::Pipeline<v_t, e_t>::apply_complete() {
  return a4->complete();
}

template<class v_t, class e_t>
void SimObj::Pipeline<v_t, e_t>::process_ready() {
  p8->flush();
  p1->ready();
}

template<class v_t, class e_t>
void SimObj::Pipeline<v_t, e_t>::apply_ready() {
  a4->flush();
  a1->ready();
}

template<class v_t, class e_t>
void SimObj::Pipeline<v_t, e_t>::print_stats() {

}
