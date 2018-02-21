cmake_minimum_required(VERSION 3.5)
project(immutable LANGUAGES CXX)

find_package(immutable-vector REQUIRED)
find_package(immutable-optional REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    immutable-vector::immutable-vector
    immutable-optional::immutable-optional
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
