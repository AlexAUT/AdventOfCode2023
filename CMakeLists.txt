cmake_minimum_required(VERSION 3.27.9)

project(AoC2023)

# Dependencies managed by vcpkg
find_package(fmt CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

add_subdirectory(./scnlib)

function(add_common_properties TARGET)
  target_compile_options(
    ${TARGET}
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror -DGLIBCXX_DEBUG>)

  set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 23)

  target_link_libraries(${TARGET} PRIVATE fmt::fmt Catch2::Catch2
                                          Catch2::Catch2WithMain scn::scn)
  target_include_directories(${TARGET} PRIVATE include)
endfunction()

add_executable(day1 day1/main.cpp)
add_common_properties(day1)

add_executable(day2 day2/main.cpp)
add_common_properties(day2)

add_executable(day3 day3/main.cpp)
add_common_properties(day3)

add_executable(day4 day4/main.cpp)
add_common_properties(day4)

add_executable(day5 day5/main.cpp)
add_common_properties(day5)

add_executable(day6 day6/main.cpp)
add_common_properties(day6)

add_executable(day7 day7/main.cpp)
add_common_properties(day7)

add_executable(day8 day8/main.cpp)
add_common_properties(day8)

add_executable(day9 day9/main.cpp)
add_common_properties(day9)

add_executable(day10 day10/main.cpp)
add_common_properties(day10)

add_executable(day11 day11/main.cpp)
add_common_properties(day11)

add_executable(day12 day12/main.cpp)
add_common_properties(day12)

add_executable(day13 day13/main.cpp)
add_common_properties(day13)
