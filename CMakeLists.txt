cmake_minimum_required(VERSION 3.31)
project(arco)

set(CMAKE_CXX_STANDARD 23)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(LLVM_LINK_COMPONENTS
        core
        analysis
        IRReader
        TransformUtils
        ScalarOpts
        codegen
        Target
        MC
        orcjit
        native
        nativecodegen
        support
        target
        executionengine
        passes
)
llvm_map_components_to_libnames(LLVM_LIBS ${LLVM_LINK_COMPONENTS})

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()



add_subdirectory(compiler)
add_subdirectory(tests)

add_executable(arco main.cpp)
target_link_libraries(arco PUBLIC
        AST
        sema
        driver
        lexer
        parser
        errors
        typing
)
