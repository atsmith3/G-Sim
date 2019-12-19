// Andrew Smith
//
// Graph class implementation
//
// 121219

#include <fstream>
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/progress.hpp>
#include <cassert>
#include <stdexcept>

namespace Utility {

template<class e_t>
Edge<e_t>::Edge(e_t init, uint64_t src, uint64_t dst) {
  this->property = init;
  this->src = src;
  this->dst = dst;
}

template<class v_t, class e_t>
std::ostream& operator<<(std::ostream& os, const Vertex<v_t, e_t>& obj) {
  os << obj.property;
  return os;
}


template<class v_t, class e_t>
void Graph<v_t, e_t>::import(std::string fname) {
  std::fstream g;
  g.open(fname, std::ios::in);
  std::string line;
  std::vector<std::string> tokens;
  uint64_t i = 0;
  uint64_t progress = 0;
  uint64_t src, dst;
  uint64_t rows, columns;
  uint64_t entries = 0;
  std::string object, format, field, symmetry;

  while(!g.eof()) {
    std::getline(g, line);
    //std::cout << line << "\n";
    boost::algorithm::trim(line);
    boost::algorithm::split(tokens, line, boost::is_any_of(" "));

    if(line == "") {
      continue;
    }
    if(i == 0) {
      if(tokens[0].find("%MatrixMarket") != std::string::npos) {
        object = tokens[1];
        assert(object == "matrix");
        format = tokens[2];
        assert(format == "coordinate");
        field = tokens[3];
        symmetry = tokens[4];
        i++;
      }
      else {
        std::runtime_error("MatrixMarket file header invalid");
      }
    }
    else {
      if(tokens[0].find("%") == 0) {
        /* Do Nothing, line is a comment */
      }
      else {
        if(i == 1) {
          // Special Line: "<rows> <cols> <non-zeros>"
          rows = std::stoull(tokens[0]);
          columns = std::stoull(tokens[1]);
          entries = std::stoull(tokens[2]);
          assert(columns == rows);
          assert(entries < columns*rows);
          vertex.resize(rows);
        }
        else {
          dst = std::stoull(tokens[0]) - 1;
          src = std::stoull(tokens[1]) - 1;
          if(field == "pattern") {
            if(symmetry == "symmetric") {
              vertex[dst].property = v_t();
              vertex[dst].edges.push_back(Edge<e_t>(1, src, dst));
              vertex[src].in_edges.push_back(Edge<e_t>(1, src, dst));
            }
            vertex[src].property = v_t();
            vertex[src].edges.push_back(Edge<e_t>(1, src, dst));
            vertex[dst].in_edges.push_back(Edge<e_t>(1, src, dst));
            progress++;
          }
          else if(field == "real") {
            double r_weight = std::stod(tokens[2]);
            if(symmetry == "symmetric") {
              vertex[dst].property = v_t();
              vertex[dst].edges.push_back(Edge<e_t>(r_weight, src, dst));
              vertex[src].in_edges.push_back(Edge<e_t>(r_weight, src, dst));
            }
            vertex[src].property = v_t();
            vertex[src].edges.push_back(Edge<e_t>(r_weight, src, dst));
            vertex[dst].in_edges.push_back(Edge<e_t>(r_weight, src, dst));
            progress++;
          }
          else if(field == "integer") {
            int i_weight = std::stoi(tokens[2]);
            if(symmetry == "symmetric") {
              vertex[dst].property = v_t();
              vertex[dst].edges.push_back(Edge<e_t>(i_weight, src, dst));
              vertex[src].in_edges.push_back(Edge<e_t>(i_weight, src, dst));
            }
            vertex[src].property = v_t();
            vertex[src].edges.push_back(Edge<e_t>(i_weight, src, dst));
            vertex[dst].in_edges.push_back(Edge<e_t>(i_weight, src, dst));
            progress++;
          }
        }
        i++;
      }
    }
    if((progress % 1000 == 0 || progress == entries) && entries != 0) {
      printf("\rImported: %3d%%", (int)((progress*100)/entries));
      fflush (stdout);
    }
  }

  std::cout << "\n";
  return;
}

template<class v_t, class e_t>
void Graph<v_t, e_t>::printVertexProperties(uint64_t n) {
  std::cout << "VertexProperties [ ";
  for(uint64_t i = 0; i < n && i < vertex.size(); i++) { 
    if(i == (n - 1) || i == (vertex.size() - 1)) {
      std::cout << vertex[i].property << " ]\n";
    }
    else {
      std::cout << vertex[i].property << ", ";
    }
  }
}

template<class v_t, class e_t>
void Graph<v_t, e_t>::writeVertexProperties(std::string fname) {
  std::fstream out;
  out.open(fname, std::ios::out);

  for(auto it = vertex.begin(); it != vertex.end(); it++) {
    out << *it << "\n";
  }

  out.close();
}

template<class v_t, class e_t>
void Graph<v_t, e_t>::serialize() {


}

template<class v_t, class e_t>
void Graph<v_t, e_t>::deserialize() {


}

}; // namespace Utility
