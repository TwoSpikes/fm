cmake_minimum_required(VERSION 3.24)

project(tsfm)

set(CMAKE_CXX_STANDARD 23)

set( SOURCE_FILES
  main.cpp
)

add_executable( tsfm
  ${SOURCE_FILES}
)
