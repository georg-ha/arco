add_library(driver
        module.h
        source_file.h
        source_file.cpp
        module.cpp
)

target_include_directories(driver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(driver
        sema
        parser
        typing
        ${LLVM_LIBS}
)