cmake_minimum_required(VERSION 3.23.0)
project(HelloOccStepToH5 VERSION 0.2.0 LANGUAGES C CXX)


set(OCCT_SDK_DIR "/opt/occt/7.8.1")
set(HDF5_SDK_DIR "/opt/hdf5/1.14")

add_executable(HelloOccStepToH5)

target_compile_features(HelloOccStepToH5 PUBLIC cxx_std_20)


# include path
target_include_directories(HelloOccStepToH5 PUBLIC
  "${OCCT_SDK_DIR}/include/opencascade"
  "${HDF5_SDK_DIR}/include"
)

# source tree
list(APPEND private_header_list
    HelloOccStepToH5.h
)
list(APPEND base_source_list
  WriteStepAttributeHdf5.cpp
)
source_group("Header Files" FILES ${private_header_list})
source_group("Source Files" FILES ${base_source_list})

set(build_source_list)
list(APPEND build_source_list ${base_source_list})
target_sources(HelloOccStepToH5
   PRIVATE
   ${build_source_list}
  
  PRIVATE
  FILE_SET private_headers
  TYPE HEADERS
  BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${private_header_list}
)


# dependent library directories
list(APPEND DEPENDENT_LIB_DIRS 
  "${OCCT_SDK_DIR}/lib"
  "${HDF5_SDK_DIR}/lib"
)
target_link_directories(HelloOccStepToH5 PUBLIC "${DEPENDENT_LIB_DIRS}")


# OCC dependencies
set(OCCT_LIBS
     TKernel
    TKService
    TKMath
    TKBO
    TKV3d
    # TKOpenGl
    TKBRep
    TKGeomBase
    TKGeomAlgo
    TKMesh
    TKRWMesh
    TKXSBase
    TKG3d
    TKG2d
    TKTopAlgo
    TKPrim
    TKDECascade
    TKDESTEP
    TKDE
    TKTObj
    TKDEOBJ
    TKBin
    TKDESTL
    TKStdL
    TKStd
    TKDCAF
    TKLCAF
    TKXCAF
    TKVCAF
    TKShHealing
    TKHLR
  )

  # HDF5 dependencies
set(HDF5_LIBS
   hdf5
   hdf5_cpp
)

list(APPEND DEPENDENT_LIBS 
   ${OCCT_LIBS}
   ${HDF5_LIBS}
)
target_link_libraries(HelloOccStepToH5 PUBLIC ${DEPENDENT_LIBS})