cmake_minimum_required(VERSION 3.1)
project(JAMReader)

set(CMAKE_CXX_STANDARD 14)

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

ROOT_GENERATE_DICTIONARY(
        G__GenModel GenModel.h
        LINKDEF GenModelLinkDef.h
)

add_library(GenModel SHARED GenModel.h GenModel.cpp G__GenModel.cxx)
target_link_libraries(GenModel PRIVATE ${ROOT_LIBRARIES})


add_executable(JAMReader main.cpp)
target_link_libraries(JAMReader PRIVATE GenModel ${ROOT_LIBRARIES})

