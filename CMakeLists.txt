cmake_minimum_required(VERSION 3.20)

project(lab001 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
include(CTest) 

set(SQLITECPP_INTERNAL_SQLITE ON CACHE BOOL "Build bundled SQLite" FORCE)
set(SQLITECPP_BUILD_TESTS OFF CACHE BOOL "No SQLiteCpp tests" FORCE)
set(SQLITECPP_BUILD_EXAMPLES OFF CACHE BOOL "No SQLiteCpp examples" FORCE)
FetchContent_Declare(sqlitecpp
    GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG 3.3.3
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(sqlitecpp)


FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

add_library(casino_lib
    src/Bet.cpp
    src/DomainError.cpp 
    src/FileLogger.cpp
    src/GameService.cpp
    src/Money.cpp 
    src/Player.cpp
    src/PlayerId.cpp 
    src/RealRandomGenerator.cpp
    src/RouletteNumber.cpp
    src/Table.cpp
    src/Tablelimits.cpp
    src/BotStrategy.cpp

)
target_include_directories(casino_lib PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(casino_lib PRIVATE SQLiteCpp)

add_executable(lab001 src/Casino.cpp) 
target_compile_features(lab001 PRIVATE cxx_std_20)
target_link_libraries(lab001 PRIVATE casino_lib)

enable_testing()
add_subdirectory(tests)
