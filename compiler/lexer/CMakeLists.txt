add_library(lexer
        lexer.h
        location.h
        token.h
        token_type.h
        token_type.cpp
        lexer.cpp
        location.cpp
        token.cpp
)

target_include_directories(lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(lexer driver errors)