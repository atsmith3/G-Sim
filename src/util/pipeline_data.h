/*
 *
 * Andrew Smith
 * Struct for the data that needs to move through the pipeline
 *
 * 09/28/19
 */

#ifndef PIPELINE_DATA_H
#define PIPELINE_DATA_H

#include <iostream>

namespace Utility {

// v_t is the data type of the vertex
// e_t is the data type of the edge
template<class v_t, class e_t>
struct pipeline_data {
  uint64_t vertex_id;
  uint64_t vertex_dst_id;
  uint64_t edge_id;

  v_t vertex_data;
  v_t vertex_dst_data;
  v_t message_data;
  v_t vertex_temp_dst_data;

  e_t edge_data;
  e_t edge_temp_data;

  bool last_vertex;
  bool last_edge;

  bool updated;

  pipeline_data() {
    vertex_id = 0;
    vertex_dst_id = 0;
    edge_id = 0;

    vertex_data = v_t();
    vertex_dst_data = v_t();
    message_data = v_t();
    vertex_temp_dst_data = v_t();

    edge_data = e_t();
    edge_temp_data = e_t();

    last_vertex = false;
    last_edge = false;

    updated = false;
  }
};

template<class v_t, class e_t>
std::ostream& operator<<(std::ostream& out, const pipeline_data<v_t, e_t>& obj) {
  out << "{ vertex_id:" << obj.vertex_id << ", vertex_dst_id:" << obj.vertex_dst_id << ", edge_id:" << obj.edge_id; 
  out << ", vertex_data:" << obj.vertex_data << ", vertex_dst_data:" << obj.vertex_dst_data << ", message_data:" << obj.message_data << ", vertex_temp_dst_data:" << obj.vertex_temp_dst_data;
  out << ", edge_data:" << obj.edge_data << ", edge_temp_data:" << obj.edge_temp_data;
  out << ", last_vertex:" << obj.last_vertex << ", last_edge:" << obj.last_edge << ", updated:" << obj.updated << " }";
  return out;
}

}; // namepsace Utility

#endif // PIPELINE_DATA_H
