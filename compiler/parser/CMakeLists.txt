add_library(parser
        parser.h
        parser.cpp
        expr_parser.cpp
        stmt_parser.cpp
)

target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(parser lexer AST errors)