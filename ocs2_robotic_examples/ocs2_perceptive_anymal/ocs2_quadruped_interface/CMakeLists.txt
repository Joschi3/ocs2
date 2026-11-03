cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(ocs2_quadruped_interface)

## ament_cmake Dependencies
set(dependencies
  ocs2_core
  ocs2_ddp
  ocs2_mpc
  ocs2_sqp
  ocs2_ros_interfaces
  ocs2_robotic_tools
  ocs2_switched_model_interface
  ocs2_anymal_commands
  rclcpp
  tf2
  kdl_parser
  robot_state_publisher
  segmented_planes_terrain_model
)
find_package(ament_cmake REQUIRED)
find_package(ocs2_core REQUIRED)
find_package(ocs2_ddp REQUIRED)
find_package(ocs2_mpc REQUIRED)
find_package(ocs2_sqp REQUIRED)
find_package(ocs2_ros_interfaces REQUIRED)
find_package(ocs2_robotic_tools REQUIRED)
find_package(ocs2_switched_model_interface REQUIRED)
find_package(ocs2_anymal_commands REQUIRED)
find_package(rclcpp REQUIRED)
find_package(tf2 REQUIRED)
find_package(kdl_parser REQUIRED)
find_package(robot_state_publisher REQUIRED)
find_package(segmented_planes_terrain_model REQUIRED)
find_package(PCL REQUIRED)

## Eigen3
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

find_package(Boost REQUIRED COMPONENTS
  filesystem
  log_setup
  log
)




###########
## Build ##
###########

include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
  ${PCL_INCLUDE_DIRS}
)

# Declare a C++ library
set(dependencies_targets
  ${ocs2_core_TARGETS}
  ${ocs2_ddp_TARGETS}
  ${ocs2_mpc_TARGETS}
  ${ocs2_sqp_TARGETS}
  ${ocs2_ros_interfaces_TARGETS}
  ${ocs2_robotic_tools_TARGETS}
  ${ocs2_switched_model_interface_TARGETS}
  ${ocs2_anymal_commands_TARGETS}
  ${rclcpp_TARGETS}
  ${tf2_TARGETS}
  ${kdl_parser_TARGETS}
  ${robot_state_publisher_TARGETS}
  ${segmented_planes_terrain_model_TARGETS}
)

add_library(${PROJECT_NAME}
  src/QuadrupedDummyNode.cpp
  src/QuadrupedInterface.cpp
  src/QuadrupedLogger.cpp
  src/QuadrupedMpc.cpp
  src/QuadrupedMpcNode.cpp
  src/QuadrupedPointfootInterface.cpp
  src/QuadrupedTfPublisher.cpp
  src/QuadrupedVisualizer.cpp
  src/SwingPlanningVisualizer.cpp
  src/TerrainPlaneVisualizer.cpp
  src/TerrainReceiver.cpp
)
target_link_libraries(${PROJECT_NAME} ${dependencies_targets})
target_compile_options(${PROJECT_NAME} PUBLIC ${OCS2_CXX_FLAGS})

#############
## Install ##
#############

install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include/${PROJECT_NAME}
)
install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(DIRECTORY config launch
  DESTINATION share/${PROJECT_NAME}/
)

ament_export_dependencies(${dependencies})
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_package()
