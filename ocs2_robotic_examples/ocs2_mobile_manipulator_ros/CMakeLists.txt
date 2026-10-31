cmake_minimum_required(VERSION 3.10)
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

# System deps: linked separately
find_package(Boost REQUIRED COMPONENTS filesystem log_setup log)
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

set(ament_deps_targets
  ${rclcpp_TARGETS}
  ${tf2_ros_TARGETS}
  ${urdf_TARGETS}
  ${kdl_parser_TARGETS}
  ${robot_state_publisher_TARGETS}
  ${visualization_msgs_TARGETS}
  ${geometry_msgs_TARGETS}
  ${ocs2_ros_interfaces_TARGETS}
  ${ocs2_core_TARGETS}
  ${ocs2_ddp_TARGETS}
  ${ocs2_mpc_TARGETS}
  ${ocs2_robotic_tools_TARGETS}
  ${ocs2_robotic_assets_TARGETS}
  ${ocs2_pinocchio_interface_TARGETS}
  ${ocs2_self_collision_TARGETS}
  ${ocs2_self_collision_visualization_TARGETS}
  ${ocs2_mobile_manipulator_TARGETS}
)

# MPC node
add_executable(mobile_manipulator_mpc_node
  src/MobileManipulatorMpcNode.cpp
)
target_link_libraries(mobile_manipulator_mpc_node ${ament_deps_targets})
target_compile_options(mobile_manipulator_mpc_node PUBLIC ${FLAGS})
# Keep target_link_libraries "plain"
target_link_libraries(mobile_manipulator_mpc_node ${Boost_LIBRARIES})

# DistanceVisualization node
add_executable(mobile_manipulator_distance_visualization
  src/MobileManipulatorDistanceVisualization.cpp
)
target_link_libraries(mobile_manipulator_distance_visualization ${ament_deps_targets})
target_compile_options(mobile_manipulator_distance_visualization PUBLIC ${FLAGS})
target_link_libraries(mobile_manipulator_distance_visualization ${Boost_LIBRARIES})

# Dummy node
add_executable(mobile_manipulator_dummy_mrt_node
  src/MobileManipulatorDummyMRT.cpp
  src/MobileManipulatorDummyVisualization.cpp
)
target_link_libraries(mobile_manipulator_dummy_mrt_node ${ament_deps_targets})
target_compile_options(mobile_manipulator_dummy_mrt_node PUBLIC ${FLAGS})
target_link_libraries(mobile_manipulator_dummy_mrt_node ${Boost_LIBRARIES})

# Target node
add_executable(mobile_manipulator_target
  src/MobileManipulatorTarget.cpp
)
target_link_libraries(mobile_manipulator_target ${ament_deps_targets})
target_compile_options(mobile_manipulator_target PUBLIC ${FLAGS})
target_link_libraries(mobile_manipulator_target ${Boost_LIBRARIES})


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
