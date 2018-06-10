cmake_minimum_required(VERSION 3.10)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 11)

add_executable(MiniSQL main.cpp Test.cpp Test.h Interpreter.cpp Interpreter.h MiniType.h API.cpp API.h CatalogManager.cpp CatalogManager.h RecordManager.cpp RecordManager.h IndexManager.cpp IndexManager.h BufferManager.cpp BufferManager.h BPlusTree.cpp BPlusTree.h)