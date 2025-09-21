add_library(sema
        symbol.h
        symbol.cpp
        scope.cpp
        scope.h
        module_collector.h
        module_collector.cpp
        name_resolution.h
        name_resolution.cpp
)

target_include_directories(sema PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(sema PUBLIC driver AST errors)