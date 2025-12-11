cmake_minimum_required(VERSION 3.15)

project(
  Edit-Distance
  VERSION 1.0
  DESCRIPTION "Hamming Distance Randomized Algorithms Project"
  HOMEPAGE_URL "https://github.com/Balint-R/6_5220-project/"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-O3)
add_compile_options(-Wall)
add_compile_options(-Wshadow)

add_executable(testing_framework
    src/testing_framework.cpp
    src/ham_dist_bf.cpp
    src/ham_dist_bf_fast.cpp
    src/ham_dist_proj.cpp
    src/ham_dist_proj_sc.cpp
    src/ham_dist_sqrt.cpp
    src/first_alg_with_heuristics.cpp
    src/HAM.cpp
    src/convolution.cpp
)

target_include_directories(testing_framework
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
