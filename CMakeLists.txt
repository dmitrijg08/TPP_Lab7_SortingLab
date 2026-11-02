cmake_minimum_required(VERSION 3.21)
project(SortingLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Статична бібліотека сортування масивів (Варіант 9)
add_library(SortLib STATIC
    src/SortableArray.cpp
)

target_include_directories(SortLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Демонстраційна програма
add_executable(SortDemo
    demo/main.cpp
)
target_link_libraries(SortDemo
    PRIVATE SortLib
)

# Демонстрація антипатернів обробки винятків
add_executable(AntipatternDemo
    antipattern/main.cpp
)
target_link_libraries(AntipatternDemo
    PRIVATE SortLib
)

# Модульне тестування Google Test
find_package(GTest CONFIG REQUIRED)
enable_testing()

add_executable(SortTests
    tests/SortTests.cpp
)
target_link_libraries(SortTests
    PRIVATE
        SortLib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(SortTests)
