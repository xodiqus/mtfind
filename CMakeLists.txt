cmake_minimum_required(VERSION 3.5)

project(mtfind LANGUAGES CXX)

file(COPY "input.txt" DESTINATION "${CMAKE_BINARY_DIR}/")

add_library(mtfind_lib mtfind.cpp mtfind.h)
add_executable(mtfind main.cpp input.txt)

foreach (proj mtfind mtfind_lib)
        target_compile_features(${proj} PRIVATE cxx_std_20)
endforeach()

target_link_libraries(mtfind PRIVATE mtfind_lib)

install(TARGETS mtfind
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

find_package(Boost COMPONENTS unit_test_framework)

if (${BOOST_FOUND})
        add_executable(mtfind_test test_mtfind.cpp test_mtfind.h)
        target_compile_features(mtfind_test PRIVATE cxx_std_20)
        target_link_libraries(mtfind_test PRIVATE mtfind_lib)
        target_include_directories(mtfind_test PRIVATE ${BOOST_INCLUDE_DIR})
endif()
