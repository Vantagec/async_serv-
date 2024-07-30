cmake_minimum_required(VERSION 3.6)

project(async LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(${PROJECT_NAME} SHARED
    include/async.h
    src/async.cpp
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    bulklib
)

target_include_directories(${PROJECT_NAME}
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include  
)
