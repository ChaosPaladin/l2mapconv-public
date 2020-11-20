cmake_minimum_required(VERSION 3.17)
project(geodata)

add_library(${PROJECT_NAME}
    src/pch.cpp
    src/L2JSerializer.cpp
    src/Loader.cpp
    src/Exporter.cpp
    src/Map.cpp
    src/Builder.cpp
    src/Preprocessing.cpp
    src/ExportBuffer.cpp
    src/Optimizer.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)

target_link_libraries(${PROJECT_NAME}
    PRIVATE utils
    PRIVATE math
    PRIVATE llvm

    PRIVATE glm
    PRIVATE Recast
)

# Compiler options
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
