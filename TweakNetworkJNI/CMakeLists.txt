# CMakeList.txt : CMake project for TweakNetworkCMake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(tweaknetworkjni)

#find_package(Java 1.8) 
#if (Java_FOUND)  
#    message (STATUS "Java_VERSION=${Java_VERSION}")
#endif()

find_package(JNI)
if (JNI_FOUND)  
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
endif()

set(tweaknetworkjni_source_files
  "src/com_example_network_tweaker_NetworkTweakerBase.cpp"
)

add_library (${PROJECT_NAME} SHARED ${tweaknetworkjni_source_files})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PUBLIC ${JNI_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    all::tweaknetworkbase
    ${JNI_LIBRARIES}
)

add_custom_command(TARGET ${PROJECT_NAME} 
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_SOURCE_DIR}/NetworkTweakerApp/build
)