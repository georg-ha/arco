add_library(typing
        type.h
        type.cpp
        type_checker.h
        type_checker.cpp
        format_specifier.h
        format_specifier.cpp
)

target_include_directories(typing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(typing AST)