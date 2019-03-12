#pragma once
#include "readImageToMat.h"
#include "Picture.h"

//If filename contain an image filename, then it gets loaded as needed; otherwise mat is used to stores the image

class OpenCVPicture : public Picture {
  float scaleUCharColor(float col);
public:
  int xOffset; //Shift to the right
  int yOffset; //Shift down
  int backgroundColor;
  int scale;
  float scale2;
  float scale2xx, scale2xy,scale2yy;
  cv::Mat mat;
  std::string filename;

  OpenCVPicture(int xSize, int ySize, int nInputFeatures, unsigned char backgroundColor,int label_ = -1);
  OpenCVPicture(std::string filename, int scale=256, unsigned char backgroundColor=128, int label_ = -1);
  ~OpenCVPicture();
  Picture* distort (RNG& rng, batchType type=TRAINBATCH);
  void affineTransform(float c00, float c01, float c10, float c11);
  void centerMass();
  void codifyInputData(SparseGrid &grid, std::vector<float> &features, int &nSpatialSites, int spatialSize);
  void jiggle(RNG &rng, int offlineJiggle);
  void jiggleFit(RNG &rng, int subsetSize, float minFill=-1);
  void colorDistortion(RNG &rng, int sigma1, int sigma2, int sigma3, int sigma4);
  void loadData  (int scale_=-1);
  void loadDataWithoutScaling(int flag=-1);
  void randomCrop(RNG &rng, int subsetSize);
  std::string identify();
};

void matrixMul2x2inPlace(float& c00, float& c01, float& c10, float& c11, float a00, float a01, float a10, float a11);
