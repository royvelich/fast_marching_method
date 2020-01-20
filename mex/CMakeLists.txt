cmake_minimum_required(VERSION 3.0)
project(fmm)

# use c++ 11
set(CMAKE_CXX_STANDARD 11)

find_package(Matlab)

matlab_add_mex(NAME fmm SRC src/fmm.cpp)