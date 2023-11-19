cmake_minimum_required(VERSION 3.0.0)
project(sidescroller VERSION 0.1.0)

include(CTest)
enable_testing()


add_executable(sidescroller main.cpp bmp.h 2dgame.h )
target_link_libraries(sidescroller SDL2 SDL2main GL)
target_include_directories(sidescroller PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
