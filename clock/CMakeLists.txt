add_library(clock
    time.cpp
    )
target_compile_definitions(clock PRIVATE
    BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG
    )
target_link_libraries(clock
    Boost::date_time
    outcome
    )
