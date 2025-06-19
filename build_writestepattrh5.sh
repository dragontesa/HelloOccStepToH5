#!/usr/bin/sh
OCC_SDK=/opt/occt/7.8.1/
HDF5_SDK=/opt/hdf5/1.14
gcc -std=c++20 WriteStepAttributeHdf5.cpp -o step2hdf5 \
  -std=c++20 \
  -I$OCC_SDK/include/opencascade \
  -I$HDF5_SDK/include \
  -L$OCC_SDK/lib -lstdc++ -lTKDESTEP -lTKXCAF -lTKCAF -lTKernel -lTKXSBase -lTKShHealing \
  -L$HDF5_SDK/lib -lhdf5_cpp -lhdf5


## -L$OCC_SDK/lib -lstdc++ -lTKSTEPCAF -lTKXCAF -lTKCAF -lTKernel -lTKSTEP -lTKXSBase -lTKShHealing \