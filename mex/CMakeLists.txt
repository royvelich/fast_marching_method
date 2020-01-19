cmake_minimum_required(VERSION 3.0)
project(dist_mex)

# use c++ 11
set(CMAKE_CXX_STANDARD 11)

find_package(Matlab)

matlab_add_mex(NAME dist_mex SRC src/main.cpp)