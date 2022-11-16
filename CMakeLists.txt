cmake_minimum_required(VERSION 3.24)

project(fm)

set(CMAKE_CXX_STANDARD 23)

set( SOURCE_FILES
  main.cpp
)

add_executable( fm
  ${SOURCE_FILES}
)
