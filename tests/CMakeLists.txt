
add_executable(run_all_tests
    TestRandomGenerator.cpp
    MoneyTest.cpp
    RouletteNumberTest.cpp
    BetPayoutTest.cpp
    TableLimitsTest.cpp
    TableAggregateTest.cpp
)


target_include_directories(run_all_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(run_all_tests PRIVATE
    gtest_main
    casino_lib
)

include(GoogleTest)
gtest_discover_tests(run_all_tests)
