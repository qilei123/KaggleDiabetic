#pragma once
#include <vector>
#include "SparseGrid.h"
#include "vectorCUDA.h"

//This is a subset of the whole interface.
//It contains the larger vectors that can mostly stay on the GPU
class SpatiallySparseBatchSubInterface {
public:
  vectorCUDA<float> features;              // In the input layer, this is configured during preprocessing
  vectorCUDA<float> dfeatures;             // For the backwards/backpropagation pass
  vectorCUDA<int> poolingChoices;
  void reset() {
    features.resize(0);
    dfeatures.resize(0);
    poolingChoices.resize(0);
  }
};

class SpatiallySparseBatchInterface {
public:
  SpatiallySparseBatchSubInterface* sub;
  int nFeatures;                           // Features per spatial location
  // Not dropped out features per spatial location
  vectorCUDA<int> featuresPresent;         // For dropout rng.NchooseM(nFeatures,featuresPresent.size());
  int nSpatialSites;                       // Total active spatial locations within the
  int spatialSize;                         // spatialSize x spatialSize grid
  //                                          batchSize x spatialSize x spatialSize
  //                                          possible locations.
  bool backpropErrors;                     //Calculate dfeatures? (false until after the first NiN layer)
  std::vector<SparseGrid> grids;         // batchSize vectors of maps storing info on grids of size spatialSize x spatialSize
  //                                          Store locations of nSpatialSites in the
  //                                          spatialSize x spatialSize grids
  //                                          -1 entry corresponds to null vectors in needed
  // Below used internally for convolution/pooling operation:
  vectorCUDA<int> rules;
  SpatiallySparseBatchInterface();
  void summary();
  void reset();
};
