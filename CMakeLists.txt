cmake_minimum_required(VERSION 3.5)

project(mtfind LANGUAGES CXX)

file(COPY "input.txt" DESTINATION "${CMAKE_BINARY_DIR}/")

add_executable(mtfind 	main.cpp
                        mkfind.cpp mkfind.h
    input.txt)

target_compile_features(mtfind PRIVATE cxx_std_20)

install(TARGETS mtfind
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
