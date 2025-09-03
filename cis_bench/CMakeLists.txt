cmake_minimum_required(VERSION 3.16)
project(cis_bench LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(POPPLERCPP REQUIRED poppler-cpp)
find_package(spdlog REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

find_library(XLSXWRITER xlsxwriter REQUIRED)

add_executable(cis_bench main.cpp)
target_include_directories(cis_bench PRIVATE ${POPPLERCPP_INCLUDE_DIRS})
target_compile_options(cis_bench PRIVATE ${POPPLERCPP_CFLAGS_OTHER})
target_link_libraries(cis_bench PRIVATE
  ${POPPLERCPP_LIBRARIES}
  ${XLSXWRITER}
  spdlog::spdlog
  nlohmann_json::nlohmann_json
)