#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <map>
#include <set>

#include <filesystem>

extern "C" {
#include "mm_io.h"
}

template<class v_t>
void Utility::readGraph<v_t>::allocateGraph() {
  // Initialize node pointers
  nodePtrs = (unsigned int *)malloc((*numNodes + 2) * sizeof(unsigned int)); // Dummy for zero, plus extra at the end for M+1 bounds
  vertex_property = (v_t *)malloc((*numNodes + 1) * sizeof(v_t));
  for(int i = 0 ; i < *numNodes + 1; i ++) {
    vertex_property[i] = initialVertexValue;
  }
  nodeNeighbors = (unsigned int *)malloc(*numNeighbors * sizeof(unsigned int));
  edgeWeights = (double *)malloc(*numNeighbors * sizeof(double));
  nodeIncomingPtrs = (unsigned int *)malloc((*numNodes + 1) * sizeof(unsigned int));
  nodeIncomingNeighbors = (unsigned int *)malloc(*numNeighbors * sizeof(unsigned int));
}

template<class v_t>
void Utility::readGraph<v_t>::readMatrixMarket(const char *mmInputFile) {
  fprintf(stderr, "[readMatrixMarket] allocating space for shared integers \n");
  numNodes = (int*) malloc(sizeof(int));
  numNeighbors = (int*) malloc(sizeof(int));

  std::string binFname = std::string(mmInputFile)+".bin";
  bool success = readBin(binFname);
  if(!success) {
    int M, N, nz, *I, *J;
    double *val;
    MM_typecode matcode;
    fprintf(stderr, "[readMatrixMarket] Reading matrix market file \n");
    int retval = mm_read_mtx_crd(mmInputFile, &M, &N, &nz, &I, &J, &val, &matcode);
    if(retval < 0) {
      fprintf(stderr, "matrix read failed!\n");
      exit(-1);
    }

    // Check values: no vertices should be zero or > M
    for(int j=0; j<nz; j++){
      if((I[j] <= 0) || (J[j] <= 0)) {
        fprintf(stderr, "[readMatrixMarket] ERROR: matrix file contains an unsupported 0 vertex at %i\n", j);
        assert(false);
      }
      if(I[j] > nz) {
          fprintf(stderr, "[readMatrixMarket] ERROR: matrix file contains an out-of-bounds vertex (%i > (nz=%i)) at position %i\n", I[j], nz, j);
        assert(false);
      }
      if(J[j] > nz) {
          fprintf(stderr, "[readMatrixMarket] ERROR: matrix file contains an out-of-bounds vertex (%i > (nz=%i)) at position %i\n", I[j], nz, j);
        assert(false);
      }
    }


    *numNodes = M;
    *numNeighbors = nz;

    allocateGraph();

    bool jIsInorder = true;
    for(int j=0; j<nz-1; j++) {
      if(j%PRINT_STEP == 0) fprintf(stderr, "[readMatrixMarket] preprocessing %i/%i\n", j, nz);
      if(J[j] > J[j+1]) jIsInorder = false;
    }

    fprintf(stderr, "[readMatrixMarket] Converting matrix market to expected input \n");
    unsigned int *nodePtr = nodePtrs;
    unsigned int *nodeNeighbor = nodeNeighbors;
    double *edgeWeight = edgeWeights;
    int n = 0;
    *nodePtr = n;
    nodePtr[0] = 0; // initialize the first pointer to the start of the edge list
    if(jIsInorder) {
      //fprintf(stderr, "Test If\n");
      int curJ = 0;
      for(n = 0; n < nz; n++) {
        if(n%PRINT_STEP == 0) fprintf(stderr, "[readMatrixMarket] postprocessing %i/%i\n", n, nz);
        //printf ("Test If 1 %d\n", n);
        nodeNeighbor[n] = I[n];
        //printf ("Test If 2\n");
        if(mm_is_real(matcode))
          *(edgeWeight++) = val[n]; //Giving error
        else
          *(edgeWeight++) = 1.;
        while(curJ != J[n]) 
          nodePtr[++curJ] = n;
      }
      // Update the very last node ptr
      nodePtr[++curJ] = *numNeighbors;
    } else {
      //fprintf(stderr, " Test Else \n");
      for(int i=0; i<=M; i++) {
        for(int ind=0; ind < nz; ind++) {
          if(n%PRINT_STEP == 0) fprintf(stderr, "[readMatrixMarket] postprocessing %i, %i/%i\n", i, ind, nz);
          if(I[ind] == i) {
            *(nodeNeighbor++) = J[ind];
            if(mm_is_real(matcode))
              *(edgeWeight++) = val[ind];
            else
              *(edgeWeight++) = 1.;
            n++;
          }
        }
        *(++nodePtr) = n;
      }
      // Update the very last node pointer
      *(++nodePtr) = *numNeighbors;
    }
    //fprintf(stderr, "Test \n");
    //assert((nodeNeighbors)[0] == (nodeNeighbors)[1] == 0);

    unsigned int p = 0;
    unsigned int idx_cnt = 0;
    memset(nodeIncomingNeighbors, 0, (*numNeighbors)*sizeof(unsigned int));
    memset(nodeIncomingPtrs, 0, (*numNodes + 1)*sizeof(unsigned int));

    std::map<int, std::set<int> > incomingNodes;
    for(int nodeInd = 0; nodeInd <= M; nodeInd++) {
      if(nodeInd%PRINT_STEP == 0) fprintf(stderr, "[readMatrixMarket] postpostprocessing %i/%i\n", nodeInd, M);
      for(unsigned int neighborInd = nodePtrs[nodeInd]; neighborInd < nodePtrs[nodeInd+1]; neighborInd++) {
        int neighbor = nodeNeighbors[neighborInd];
        incomingNodes[neighbor].insert(nodeInd);
      }
    }
    for(int nodeInd = 0; nodeInd <= M; nodeInd++) {
      if(nodeInd%PRINT_STEP == 0) fprintf(stderr, "[readMatrixMarket] postpostpostprocessing %i/%i\n", nodeInd, M);
      nodeIncomingPtrs[nodeInd] = p;
      p += incomingNodes[nodeInd].size();
      for(int neighborInd : incomingNodes[nodeInd]) {
        nodeIncomingNeighbors[idx_cnt++] = neighborInd;
      }
    }
    assert(nodeIncomingPtrs[0] == 0);
    nodeIncomingPtrs[M+1] = p;


    // check some stuff
    for(int j = 0; j < M+2; j++) {
      assert((nodePtrs[j] >=0) && (nodePtrs[j] <= ((unsigned int)nz)));
      assert((nodeIncomingPtrs[j] >=0) && (nodeIncomingPtrs[j] <= ((unsigned int)nz)));
    }
    for(int j = 0; j < nz; j++) {
      assert((nodeNeighbors[j] >=1) && (nodeNeighbors[j] <= ((unsigned int)M)));
      assert((nodeIncomingNeighbors[j] >=1) && (nodeIncomingNeighbors[j] <= ((unsigned int)M)));
    }

    free(val);
    free(J);
    free(I);
    writeBin(binFname);
  }
}

template<class v_t>
void Utility::readGraph<v_t>::printEdgeWeights(void) {
  for(int i = 0; i < *numNeighbors; i++) {
    fprintf(stderr, "[readGraph DEBUG] edge %u: %lf\n", i, edgeWeights[i]);
  }
}

template<class v_t>
void Utility::readGraph<v_t>::printNodePtrs(void) {
  fprintf(stderr, "[readGraph DEBUG] nodePtrs:\n");
  for(int i = 1; i < *numNodes; i++) {
    fprintf(stderr, "                  node %u: %u\n", i, nodePtrs[i]);
  }
}

template<class v_t>
void Utility::readGraph<v_t>::printGraph(void) {
  for(int i = 1 ; i <= *numNodes; i++) {
    std::cerr << "Node: " << i << "\n";
    std::cerr << "  Property: " << getVertexProperty(i) << "\n";
    for(int j = getNodePtr(i); j < getNodePtr(i+1); j++) {
      std::cerr << "    Edge " << j << " weight " << getEdgeWeight(j) << "\n";
      std::cerr << "      Neighbor: " << getNodeNeighbor(j) << "\n";
    }
  }
}

template<class v_t>
void Utility::readGraph<v_t>::printVertexProperties(int num) {
  std::cerr << "[ ";
  for(int i = 1; i <= *numNodes && i < num; i++) {
    std::cerr << getVertexProperty(i) << ", ";
  }
  std::cerr << "]\n";
}

template<class v_t>
void Utility::readGraph<v_t>::writeBin(std::string binFname) {
  fprintf(stderr, "[writeBin] writing binary\n");
  std::ofstream binFile;
  binFile.open(binFname.c_str(), std::ios::out | std::ios::binary);
  assert(binFile.is_open());
  binFile.write((char*)numNodes, sizeof(int));
  binFile.write((char*)numNeighbors, sizeof(int));
  binFile.write((char*)nodePtrs, sizeof(unsigned int)*((*numNodes)+1));
  binFile.write((char*)nodeNeighbors, sizeof(unsigned int)*(*numNeighbors));
  binFile.write((char*)edgeWeights, sizeof(double)*(*numNeighbors));
  binFile.write((char*)nodeIncomingPtrs, sizeof(unsigned int)*((*numNodes)+1));
  binFile.write((char*)nodeIncomingNeighbors, sizeof(unsigned int)*(*numNeighbors));
  binFile.close();
}

template<class v_t>
bool Utility::readGraph<v_t>::readBin(std::string binFname) {
  std::ifstream binFile;
  binFile.open(binFname.c_str(), std::ios::in | std::ios::binary);
  if(!binFile.good()) return false;

  uint64_t fileSize = std::filesystem::file_size(binFname.c_str());

  const size_t startIdx = binFname.find_last_of("/");
  binFname.erase(0, startIdx+1);
  boost::interprocess::permissions perm;
  perm.set_unrestricted();
  if(shouldInit == 1) boost::interprocess::shared_memory_object::remove(binFname.c_str());
  graphData = boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, 
                                                        binFname.c_str(), 
                                                        boost::interprocess::read_write, 
                                                        perm);
  graphData.truncate(fileSize);
  region = boost::interprocess::mapped_region(graphData, boost::interprocess::read_write, 0, fileSize);

  uint8_t *dataPtr = (uint8_t*)region.get_address();
  numNodes = (int*)dataPtr;
  dataPtr += sizeof(int);
  numNeighbors = (int*)dataPtr;
  dataPtr += sizeof(int);
  nodePtrs = (unsigned int*)dataPtr;
  dataPtr += sizeof(unsigned int)*((*numNodes)+1);
  nodeNeighbors = (unsigned int*)dataPtr;
  dataPtr += sizeof(unsigned int)*(*numNeighbors);
  edgeWeights = (double*)dataPtr;
  dataPtr += sizeof(double)*(*numNeighbors);
  nodeIncomingPtrs = (unsigned int*)dataPtr;
  dataPtr += sizeof(unsigned int)*((*numNodes)+1);
  nodeIncomingNeighbors = (unsigned int*)dataPtr;
  dataPtr += sizeof(unsigned int)*(*numNeighbors);
  assert(dataPtr <= (uint8_t*)region.get_address() + region.get_size());
  
  if(shouldInit != 0) {
    fprintf(stderr, "[writeBin] reading binary\n");
    binFile.read((char*)numNodes, sizeof(int));
    assert(!binFile.fail());
    fprintf(stderr, "%li bytes read\n", binFile.gcount());
    binFile.read((char*)numNeighbors, sizeof(int));
    assert(!binFile.fail());
    fprintf(stderr, "%li bytes read\n", binFile.gcount());
    fprintf(stderr, "[writeBin] read numNodes(%p): %i and numNeighbors(%p): %i\n", numNodes, *numNodes, numNeighbors, *numNeighbors);
    binFile.read((char*)nodePtrs, sizeof(unsigned int)*((*numNodes)+1));
    assert(!binFile.fail());
    fprintf(stderr, "%li bytes read\n", binFile.gcount());
    binFile.read((char*)nodeNeighbors, sizeof(unsigned int)*(*numNeighbors));
    assert(!binFile.fail());
    fprintf(stderr, "%li bytes read\n", binFile.gcount());
    binFile.read((char*)edgeWeights, sizeof(double)*(*numNeighbors));
    assert(!binFile.fail());
    fprintf(stderr, "%li bytes read\n", binFile.gcount());
    binFile.read((char*)nodeIncomingPtrs, sizeof(unsigned int)*((*numNodes)+1));
    assert(!binFile.fail());
    fprintf(stderr, "%li bytes read\n", binFile.gcount());
    binFile.read((char*)nodeIncomingNeighbors, sizeof(unsigned int)*(*numNeighbors));
    assert(!binFile.fail());
    fprintf(stderr, "%li bytes read\n", binFile.gcount());
  } else fprintf(stderr, "skipping graph initialization\n");

  binFile.close();

  return true;
}
