#include "OpenCVPicture.h"
#include <cmath>

float OpenCVPicture::scaleUCharColor(float col) {
  float div=std::max(255-backgroundColor,backgroundColor);
  return (col-backgroundColor)/div;
}

OpenCVPicture::OpenCVPicture(int xSize, int ySize, int nInputFeatures, unsigned char backgroundColor,int label) :
  Picture(label), backgroundColor(backgroundColor) {
  xOffset=-xSize/2;
  yOffset=-ySize/2;
  mat.create(xSize,ySize, CV_8UC(nInputFeatures));
}
OpenCVPicture::OpenCVPicture(std::string filename, int scale, unsigned char backgroundColor, int label_) :
  filename(filename), scale(scale), backgroundColor(backgroundColor) {
  label=label_;
}

OpenCVPicture::~OpenCVPicture() {}

void OpenCVPicture::jiggle(RNG &rng, int offlineJiggle) {
  xOffset+=rng.randint(offlineJiggle*2+1)-offlineJiggle;
  yOffset+=rng.randint(offlineJiggle*2+1)-offlineJiggle;
}
void OpenCVPicture::randomCrop(RNG &rng, int subsetSize) {
  assert(subsetSize<=std::min(mat.rows,mat.cols));
  cropImage(mat, rng.randint(mat.cols-subsetSize),rng.randint(mat.rows-subsetSize), subsetSize, subsetSize);
  xOffset=yOffset=-subsetSize/2;
}
void OpenCVPicture::affineTransform(float c00, float c01, float c10, float c11) {

  transformImage(mat, backgroundColor, c00, c01, c10, c11);
  xOffset=-mat.cols/2;
  yOffset=-mat.rows/2;
}
void OpenCVPicture::jiggleFit(RNG &rng, int subsetSize, float minFill) { //subsetSize==spatialSize for codifyInputData
  int fitCtr=100; //Give up after 100 failed attempts to find a good fit
  bool goodFit=false;
  while (!goodFit and fitCtr-- >0) {
    xOffset=-rng.randint(mat.cols-subsetSize/3);
    yOffset=-rng.randint(mat.rows-subsetSize/3);
    int pointsCtr=0;
    int interestingPointsCtr=0;
    for (int X=5; X<subsetSize; X+=10) {
      for (int Y=5; Y<subsetSize-X; Y+=10) {
        int x=X-xOffset-subsetSize/3;
        int y=Y-yOffset-subsetSize/3;
        pointsCtr++;
        if (0<=x and x<mat.cols and 0<=y and y<mat.rows)
          interestingPointsCtr+=(mat.ptr()[(pointsCtr%mat.channels())+x*mat.channels()+y*mat.channels()*mat.cols]!=backgroundColor);
      }
    }
    assert(pointsCtr>=10);
    if (interestingPointsCtr>pointsCtr*0.6)
      goodFit=true;
  }
  if (!goodFit) {
    std::cout << filename << " " << std::flush;
    xOffset=-mat.cols/2-16+rng.randint(32);
    yOffset=-mat.rows/2-16+rng.randint(32);
  }
}
void OpenCVPicture::centerMass() {
  float ax=0, ay=0, axx=0, ayy=0, axy, d=0.001;
  for (int i=0; i<mat.channels(); i++) {
    for (int x=0;x<mat.cols;++x) {
      for (int y=0;y<mat.rows;++y) {
        float f=powf(backgroundColor-mat.ptr()[i+x*mat.channels()+y*mat.channels()*mat.cols],2);
        ax+=x*f;
        ay+=y*f;
        axx+=x*x*f;
        axy+=x*y*f;
        ayy+=y*y*f;
        d+=f;
      }
    }
  }
  ax/=d;
  ay/=d;
  axx/=d;
  axy/=d;
  ayy/=d;
  xOffset=-ax/2;
  yOffset=-ay/2;
  scale2xx=axx-ax*ax;
  scale2xy=axy-ax*ay;
  scale2yy=ayy-ay*ay;
  scale2=powf(scale2xx+scale2yy,0.5);
}
void OpenCVPicture::loadDataWithoutScaling(int flag) {
  readImage(filename,mat,flag);
  xOffset=-mat.cols/2;
  yOffset=-mat.rows/2;
}
void OpenCVPicture::loadData
(int scale_) {
  if (scale_==-1) scale_=scale;
  if (mat.empty()) {
    readTransformedImage(filename,mat,scale_,1,0,0,1,backgroundColor);
    xOffset=-mat.cols/2;
    yOffset=-mat.rows/2;
  }
}
std::string OpenCVPicture::identify() {
  return filename;
}

void OpenCVPicture::codifyInputData(SparseGrid &grid, std::vector<float> &features, int &nSpatialSites, int spatialSize) {
  loadData();
  for  (int i=0; i<mat.channels(); i++)
    features.push_back(0); //Background feature
  grid.backgroundCol=nSpatialSites++;
  for (int x=0; x<mat.cols; x++) {
    int X=x+xOffset+spatialSize/3;
    for (int y=0; y<mat.rows; y++) {
      int Y=y+yOffset+spatialSize/3;
      if (X>=0 && Y>=0 && X+Y<spatialSize) {
        bool flag=false;
        for (int i=0; i<mat.channels(); i++)
          if (std::abs(scaleUCharColor(mat.ptr()[i+x*mat.channels()+y*mat.channels()*mat.cols]))>0.02)
            flag=true;
        if (flag) {
          int n=X*spatialSize+Y;
          grid.mp[n]=nSpatialSites++;
          for (int i=0; i<mat.channels(); i++) {
            features.push_back
              (scaleUCharColor(mat.ptr()[i+x*mat.channels()+y*mat.channels()*mat.cols]));
          }
        }
      }
    }
  }
}


void matrixMul2x2inPlace(float& c00, float& c01, float& c10, float& c11, float a00, float a01, float a10, float a11) { //c<-c*a
  float t00=c00*a00+c01*a10;  float t01=c00*a01+c01*a11;
  float t10=c10*a00+c11*a10;  float t11=c10*a01+c11*a11;
  c00=t00;c01=t01;
  c10=t10;c11=t11;
}
