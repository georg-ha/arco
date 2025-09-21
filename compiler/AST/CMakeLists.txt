add_library(AST
        expr_nodes.h
        stmt_nodes.h
        visitor.h
        expr_nodes.cpp
        stmt_nodes.cpp
        print_visitor.cpp
        print_visitor.h
        type_anno.cpp
        type_anno.h
        expr.h
        stmt.h
)

target_link_libraries(AST
        PUBLIC
        lexer
        IR_codegen
)

target_include_directories(AST
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)