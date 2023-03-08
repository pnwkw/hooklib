add_executable(hooklib_test)
target_sources(hooklib_test PRIVATE
        main.cpp
)

target_link_libraries(hooklib_test hooklib::hooklib doctest::doctest)
