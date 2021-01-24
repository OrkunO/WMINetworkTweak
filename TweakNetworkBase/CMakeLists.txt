# CMakeList.txt : CMake project for TweakNetworkSource, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (tweaknetworkbase)

set(tweaknetworkbase_source_files
  "src/NetworkUtil.cpp"
  "src/WiFiUtil.cpp"
  "src/StringHelper.cpp"
)

add_library (${PROJECT_NAME} ${tweaknetworkbase_source_files})
add_library(all::tweaknetworkbase ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)