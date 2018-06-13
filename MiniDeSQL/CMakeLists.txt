cmake_minimum_required(VERSION 2.8)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 11)

include_directories(MiniDeSQL)

add_executable(MiniSQL
        BPTree.h
        BufferManager.cpp
        BufferManager.h
        IndexManager.cpp
        IndexManager.hpp
        MiniType.h
        RecordManager.cpp
        RecordManager.hpp
        CatalogManager.h
        CatalogManager.cpp
        API.cpp API.h Interpreter.cpp Interpreter.h TEST_Interpreter.cpp)
