cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(ocs2_thirdparty)

find_package(ament_cmake REQUIRED)

# Header-only target so downstream packages can link against ocs2_thirdparty::ocs2_thirdparty
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS ${PROJECT_NAME} EXPORT export_${PROJECT_NAME})
install(DIRECTORY include/
        DESTINATION "include/${PROJECT_NAME}/../")

ament_export_include_directories(include)
ament_export_targets(export_${PROJECT_NAME})
ament_package()
