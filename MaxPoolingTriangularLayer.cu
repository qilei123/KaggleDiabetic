#include <iostream>
#include <cassert>
#include "utilities.h"
#include "cudaUtilities.h"
#include "MaxPoolingLayer.h"
#include "MaxPoolingTriangularLayer.h"
#include "Regions.h"

MaxPoolingTriangularLayer::MaxPoolingTriangularLayer(int poolSize, int poolStride, int dimension) : poolSize(poolSize), poolStride(poolStride), dimension(dimension) {
  S=triangleSize(poolSize,dimension);
  std::cout << dimension << "D MaxPoolingTriangularLayer side-length=" << poolSize
            << " volume=" << S << " stride " << poolStride << std::endl;
}
void MaxPoolingTriangularLayer::preprocess
(SpatiallySparseBatch &batch,
 SpatiallySparseBatchInterface &input,
 SpatiallySparseBatchInterface &output) {
  assert(input.spatialSize==inSpatialSize);
  output.nFeatures=input.nFeatures;
  output.featuresPresent.hVector()=input.featuresPresent.hVector();
  output.spatialSize=outSpatialSize;
  output.nSpatialSites=0;
  output.grids.resize(batch.batchSize);
  output.backpropErrors=input.backpropErrors;
  RegularPoolingRegionsTriangular regions(inSpatialSize, outSpatialSize,dimension,poolSize, poolStride);
  for (int item=0;item<batch.batchSize;item++)
    gridRulesTriangular
      (input.grids[item],
       output.grids[item],
       regions,
       output.nSpatialSites,
       output.rules.hVector());
}
void MaxPoolingTriangularLayer::forwards
(SpatiallySparseBatch &batch,
 SpatiallySparseBatchInterface &input,
 SpatiallySparseBatchInterface &output) {
  output.sub->poolingChoices.resize(output.nSpatialSites*output.featuresPresent.size());
  output.sub->features.resize(output.nSpatialSites*output.featuresPresent.size());
  cudaCheckError();
  maxPool(input.sub->features.dPtr(),
          output.sub->features.dPtr(),
          output.rules.dPtr(),
          output.nSpatialSites,
          S,
          output.featuresPresent.size(),
          output.sub->poolingChoices.dPtr());
  cudaCheckError();
}
void MaxPoolingTriangularLayer::backwards
(SpatiallySparseBatch &batch,
 SpatiallySparseBatchInterface &input,
 SpatiallySparseBatchInterface &output,
 float learningRate,
 float momentum) {
  if (input.backpropErrors) {
    input.sub->dfeatures.resize(input.nSpatialSites*input.featuresPresent.size());
    input.sub->dfeatures.setZero(*cnnMemStream);
    maxPoolBackProp
      (input.sub->dfeatures.dPtr(), output.sub->dfeatures.dPtr(), output.nSpatialSites, output.featuresPresent.size(), output.sub->poolingChoices.dPtr());
  }
}
int MaxPoolingTriangularLayer::calculateInputSpatialSize(int outputSpatialSize) {
  outSpatialSize=outputSpatialSize;
  inSpatialSize=poolSize+(outputSpatialSize-1)*poolStride;
  std::cout << "(" << outSpatialSize <<"MP" <<inSpatialSize << ") ";
  return inSpatialSize;
}
