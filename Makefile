CC=g++
CFLAGS=--std=c++11 -O3
NVCC=nvcc
NVCCFLAGS=--std=c++11 -arch sm_20 -O3
OBJ=BatchProducer.o ConvolutionalLayer.o ConvolutionalTriangularLayer.o IndexLearnerLayer.o MaxPoolingLayer.o MaxPoolingTriangularLayer.o NetworkArchitectures.o NetworkInNetworkLayer.o Picture.o Regions.o Rng.o SigmoidLayer.o SoftmaxClassifier.o SparseConvNet.o SparseConvNetCUDA.o SpatiallySparseBatch.o SpatiallySparseBatchInterface.o SpatiallySparseDataset.o SpatiallySparseLayer.o TerminalPoolingLayer.o cudaUtilities.o readImageToMat.o types.o utilities.o vectorCUDA.o ReallyConvolutionalLayer.o vectorHash.o
LIBS=-L/data0/qilei_chen/opencv33_install_66/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lrt -lcublas -larmadillo

INCLUDEPATH += -I/data0/qilei_chen/opencv33_install_66/include \
               -I/data0/qilei_chen/opencv33_install_66/include/opencv2 \
               -I/data0/qilei_chen/opencv33_install_66/include/opencv

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDEPATH)
%.o: %.cu $(DEPS)
	$(NVCC) -c -o $@ $< $(NVCCFLAGS) $(INCLUDEPATH)

clean:
	rm *.o

kaggleDiabetes1: $(OBJ) OpenCVPicture.o SpatiallySparseDatasetKaggleDiabeticRetinopathy.o kaggleDiabetes1.o
	$(NVCC) -o kaggleDiabetes1 $(OBJ) OpenCVPicture.o SpatiallySparseDatasetKaggleDiabeticRetinopathy.o kaggleDiabetes1.o $(LIBS) $(NVCCFLAGS)
kaggleDiabetes2: $(OBJ) OpenCVPicture.o SpatiallySparseDatasetKaggleDiabeticRetinopathy.o kaggleDiabetes2.o
	$(NVCC) -o kaggleDiabetes2 $(OBJ) OpenCVPicture.o SpatiallySparseDatasetKaggleDiabeticRetinopathy.o kaggleDiabetes2.o $(LIBS) $(NVCCFLAGS)
kaggleDiabetes3: $(OBJ) OpenCVPicture.o SpatiallySparseDatasetKaggleDiabeticRetinopathy.o kaggleDiabetes3.o
	$(NVCC) -o kaggleDiabetes3 $(OBJ) OpenCVPicture.o SpatiallySparseDatasetKaggleDiabeticRetinopathy.o kaggleDiabetes3.o $(LIBS) $(NVCCFLAGS)
