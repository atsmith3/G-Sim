#ifndef READGRAPH_H
#define READGRAPH_H

#include <fstream>
#include <queue>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "option.h"

namespace Utility {

template<class v_t>
class readGraph {
  public:
    readGraph(Options const & opt) : shouldInit(opt.shouldInit) {}

    void readMatrixMarket(const char *mmInputFile);

    int getNumNodes() { return *numNodes; }
    int getNumNeighbors() { return *numNeighbors; }
    int getNodePtr(int nodeInd) { return nodePtrs[nodeInd]; }
    int getNodeIncomingPtr(int nodeInd) { return nodeIncomingPtrs[nodeInd]; }
    int getNodeNeighbor(int neighborInd) { return nodeNeighbors[neighborInd]; }
    int getNodeIncomingNeighbor(int neighborInd){return nodeIncomingNeighbors[neighborInd]; }

    double getEdgeWeight(int neighborInd) { return edgeWeights[neighborInd]; }

    v_t getVertexProperty(int nodeInd) { return vertex_property[nodeInd]; }
    uint64_t getVertexAddress(int nodeInd) { return (uint64_t)&vertex_property[nodeInd]; }
    void setVertexProperty(int nodeInd, v_t vertexProperty) { vertex_property[nodeInd] = vertexProperty; }
    void setInitializer(v_t initialValue) { initialVertexValue = initialValue; }
    v_t getInitializer() { return initialVertexValue; }

    std::queue<uint>* getNeighbors(int nodeInd) {
      uint *startPtr = nodeNeighbors + getNodePtr(nodeInd);
      uint *endPtr = nodeNeighbors + getNodePtr(nodeInd+1);
      std::queue<uint> *retval = new std::queue<uint>;
      while (startPtr != endPtr) retval->push(*(startPtr++));
      return retval;
    }
    std::queue<uint>* getEdges(int nodeInd) {
      uint start = getNodePtr(nodeInd);
      uint end = getNodePtr(nodeInd+1);
      std::queue<uint> *retval = new std::queue<uint>;
      while (start != end) retval->push(start++);
      return retval;
    }
    std::queue<uint>* getIncomingNeighbors(int nodeInd) {
      uint *startPtr = nodeIncomingNeighbors + getNodeIncomingPtr(nodeInd);
      uint *endPtr = nodeIncomingNeighbors + getNodeIncomingPtr(nodeInd+1);
      std::queue<uint> *retval = new std::queue<uint>;
      while (startPtr != endPtr) retval->push(*(startPtr++));
      return retval;
    }

    void printEdgeWeights(void);
    void printGraph(void);
    void printNodePtrs(void);
    void printVertexProperties(int num);

    void writeVertexPropertyToFile(std::string name) {
      std::ofstream out;
      out.open(name, std::ios::out);
      for(int i = 1; i < *numNodes + 1; i++) {
        out << vertex_property[i] << "\n";
      }
      out.close();
    }

  private:
    unsigned int *nodePtrs;
    unsigned int *nodeNeighbors;
    unsigned int *nodeIncomingPtrs;
    unsigned int *nodeIncomingNeighbors;
    double *edgeWeights;
    int *numNodes;
    int *numNeighbors;
    v_t *vertex_property;
    v_t initialVertexValue;

    int shouldInit;

    boost::interprocess::shared_memory_object graphData;
    boost::interprocess::mapped_region region;

    bool readBin(std::string binFname);
    void writeBin(std::string binFname);

    void allocateGraph();
};

}; // namespace Utility

#include "readGraph.tcc"

#endif
