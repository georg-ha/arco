add_library(IR_codegen
        codegen_visitor.h
        expr_codegen.cpp
        stmt_codegen.cpp
)

target_include_directories(IR_codegen PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(IR_codegen PRIVATE
        AST
        typing
        sema
        driver
        ${LLVM_LIBS}
)
