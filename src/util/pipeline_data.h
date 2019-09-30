/*
 *
 * Andrew Smith
 * Struct for the data that needs to move through the pipeline
 *
 * 09/28/19
 */

#ifndef PIPELINE_DATA_H
#define PIPELINE_DATA_H

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
};

}; // namepsace Utility

#endif // PIPELINE_DATA_H
