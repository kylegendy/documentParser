cmake_minimum_required(VERSION 3.15)
project(WritAnalyzer)

set(CMAKE_CXX_STANDARD 20)

add_executable(WritAnalyzer main.cpp WritAnalyzer.cpp WritAnalyzer.h GrandTrie.cpp GrandTrie.h ArtAnalyzer.cpp ArtAnalyzer.h)