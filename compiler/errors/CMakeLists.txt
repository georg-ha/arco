add_library(errors
        error_msg.h
        error_msg.cpp
        syntax_error.h
        module_collector_error.h
        double_definition_error.h
        unknown_id_error.h
        anno_mismatch_error.h
        type_error.h
        token_error.h
)

target_include_directories(errors PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})