cmake_minimum_required(VERSION 2.6) 
project(filesystem) 
add_definitions(-std=c++11)

add_executable(filesystem main.cpp FileSystem.cpp directory.cpp file.cpp indirect.cpp inode.cpp) 

