add_library(tinycbor
    tinycbor/src/cborerrorstrings.c
    tinycbor/src/cborencoder.c
    tinycbor/src/cborencoder_close_container_checked.c
    tinycbor/src/cborparser.c
    tinycbor/src/cborparser_dup_string.c
    tinycbor/src/cborpretty.c
    tinycbor/src/cborpretty_stdio.c
    tinycbor/src/cborvalidation.c
    )
target_include_directories(tinycbor PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tinycbor/src> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/deps/tinycbor/src>)
disable_clang_tidy(tinycbor)
