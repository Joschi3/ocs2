cmake_minimum_required(VERSION 3.5)
project(ocs2_mobile_manipulator_ros)
set(CMAKE_CXX_STANDARD 17)

# Ament/ROS and OCS2 deps (only ament packages go here)
set(ament_deps
  rclcpp
  tf2_ros
  urdf
  kdl_parser
  robot_state_publisher
  visualization_msgs
  geometry_msgs
  ocs2_ros_interfaces
  ocs2_core
  ocs2_ddp
  ocs2_mpc
  ocs2_robotic_tools
  ocs2_robotic_assets
  ocs2_pinocchio_interface
  ocs2_self_collision
  ocs2_self_collision_visualization
  ocs2_mobile_manipulator
)

find_package(ament_cmake REQUIRED)
foreach(pkg IN LISTS ament_deps)
  find_package(${pkg} REQUIRED)
endforeach()

# System deps: handled outside ament_target_dependencies
find_package(Boost REQUIRED COMPONENTS system filesystem log_setup log)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

###########
## Build ##
###########

set(FLAGS
  ${OCS2_CXX_FLAGS}
  -Wno-ignored-attributes
  -Wno-invalid-partial-specialization   # silence warning for unsupported Eigen Tensor
)

# Global includes only for local headers and common third-party includes
include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

# MPC node
add_executable(mobile_manipulator_mpc_node
  src/MobileManipulatorMpcNode.cpp
)
ament_target_dependencies(mobile_manipulator_mpc_node ${ament_deps})
target_compile_options(mobile_manipulator_mpc_node PUBLIC ${FLAGS})
# Keep target_link_libraries "plain" to match ament_target_dependencies usage
target_link_libraries(mobile_manipulator_mpc_node ${Boost_LIBRARIES})

# DistanceVisualization node
add_executable(mobile_manipulator_distance_visualization
  src/MobileManipulatorDistanceVisualization.cpp
)
ament_target_dependencies(mobile_manipulator_distance_visualization ${ament_deps})
target_compile_options(mobile_manipulator_distance_visualization PUBLIC ${FLAGS})
target_link_libraries(mobile_manipulator_distance_visualization ${Boost_LIBRARIES})

# Dummy node
add_executable(mobile_manipulator_dummy_mrt_node
  src/MobileManipulatorDummyMRT.cpp
  src/MobileManipulatorDummyVisualization.cpp
)
ament_target_dependencies(mobile_manipulator_dummy_mrt_node ${ament_deps})
target_compile_options(mobile_manipulator_dummy_mrt_node PUBLIC ${FLAGS})
target_link_libraries(mobile_manipulator_dummy_mrt_node ${Boost_LIBRARIES})

# Target node
add_executable(mobile_manipulator_target
  src/MobileManipulatorTarget.cpp
)
ament_target_dependencies(mobile_manipulator_target ${ament_deps})
target_compile_options(mobile_manipulator_target PUBLIC ${FLAGS})
target_link_libraries(mobile_manipulator_target ${Boost_LIBRARIES})

####################
## Clang tooling ###
####################

find_package(cmake_clang_tools QUIET)
if(cmake_clang_tools_FOUND)
  message(STATUS "Run clang tooling")
  add_clang_tooling(
    TARGETS
      mobile_manipulator_mpc_node
      mobile_manipulator_distance_visualization
      mobile_manipulator_dummy_mrt_node
      mobile_manipulator_target
    SOURCE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include
    CT_HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    CF_WERROR
  )
endif()

#############
## Install ##
#############

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

install(
  TARGETS
    mobile_manipulator_mpc_node
    mobile_manipulator_distance_visualization
    mobile_manipulator_dummy_mrt_node
    mobile_manipulator_target
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
install(DIRECTORY rviz DESTINATION share/${PROJECT_NAME})

ament_export_dependencies(${ament_deps})
ament_export_include_directories("include/${PROJECT_NAME}")
ament_package()
