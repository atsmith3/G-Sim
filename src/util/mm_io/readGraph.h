#ifndef READGRAPH_H
#define READGRAPH_H

#include <fstream>
#include <queue>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "option.h"

namespace Utility {

template<class vertex_t>
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

    vertex_t getVertexProperty(int nodeInd) { return vertex_property[nodeInd]; }
    void setVertexProperty(int nodeInd, vertex_t vertexProperty) { vertex_property[nodeInd] = vertexProperty; }
    void setInitilizer(vertex_t initialValue) { initialVertexValue = initialValue; }
    vertex_t getInitializer() { return initialVertexValue; }

    std::queue<uint>* getNeighbors(int nodeInd) {
      uint *startPtr = nodeNeighbors + getNodePtr(nodeInd);
      uint *endPtr = nodeNeighbors + getNodePtr(nodeInd+1);
      std::queue<uint> *retval = new std::queue<uint>;
      while (startPtr != endPtr) retval->push(*(startPtr++));
      return retval;
    }
    std::queue<uint>* getIncomingNeighbors(int nodeInd) {
      uint *startPtr = nodeIncomingNeighbors + getNodeIncomingPtr(nodeInd);
      uint *endPtr = nodeIncomingNeighbors + getNodeIncomingPtr(nodeInd+1);
      std::queue<uint> *retval = new std::queue<uint>;
      while (startPtr != endPtr) retval->push(*(startPtr++));
      return retval;
    }

    // debug
    void printEdgeWeights(void);
    void printGraph(void);
    void printNodePtrs(void);

  private:
    unsigned int *nodePtrs;
    unsigned int *nodeNeighbors;
    unsigned int *nodeIncomingPtrs;
    unsigned int *nodeIncomingNeighbors;
    double *edgeWeights;
    int *numNodes;
    int *numNeighbors;
    int *vertex_property;
    vertex_t initialVertexValue;

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
