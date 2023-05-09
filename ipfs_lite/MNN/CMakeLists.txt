add_library(MNN_wrapper
    MNN_parsing.cpp
    MNN_error.cpp
    )
target_link_libraries(MNN_wrapper
    libMNN
    logger
    )