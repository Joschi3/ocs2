cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(ocs2_self_collision_visualization)




set(dependencies
  ocs2_core
  ocs2_robotic_tools
  ocs2_pinocchio_interface
  ocs2_self_collision
  ocs2_ros_interfaces

  # visualization deps
  rclcpp
  std_msgs
  geometry_msgs
  visualization_msgs
  coal
  pinocchio
)

find_package(ament_cmake REQUIRED)
find_package(ocs2_core REQUIRED)
find_package(ocs2_robotic_tools REQUIRED)
find_package(ocs2_pinocchio_interface REQUIRED)
find_package(ocs2_self_collision REQUIRED)
find_package(ocs2_ros_interfaces REQUIRED)

find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)

find_package(PkgConfig REQUIRED)
find_package(pinocchio REQUIRED)
find_package(coal REQUIRED)
# requires liboctomap-dev and libassimp-dev

find_package(Boost REQUIRED COMPONENTS
  filesystem
  log_setup
  log
)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

###########
## Build ##
###########

set(FLAGS
  ${OCS2_CXX_FLAGS}
  ${pinocchio_CFLAGS_OTHER}
  -Wno-ignored-attributes
  -Wno-invalid-partial-specialization   # to silence warning with unsupported Eigen Tensor
  -DPINOCCHIO_URDFDOM_TYPEDEF_SHARED_PTR
  -DPINOCCHIO_URDFDOM_USE_STD_SHARED_PTR
)

include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
  ${pinocchio_INCLUDE_DIRS}
  ${coal_INCLUDE_DIRS}
)

set(dependencies_targets
  ${ocs2_core_TARGETS}
  ${ocs2_robotic_tools_TARGETS}
  ${ocs2_pinocchio_interface_TARGETS}
  ${ocs2_self_collision_TARGETS}
  ${ocs2_ros_interfaces_TARGETS}
  ${rclcpp_TARGETS}
  ${std_msgs_TARGETS}
  ${geometry_msgs_TARGETS}
  ${visualization_msgs_TARGETS}
  coal::coal
  pinocchio::pinocchio
)

# ocs2 pinocchio interface library
add_library(${PROJECT_NAME}
  src/GeometryInterfaceVisualization.cpp
)
target_link_libraries(${PROJECT_NAME} ${dependencies_targets})
target_compile_options(${PROJECT_NAME} PUBLIC ${FLAGS})


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

ament_export_dependencies(${dependencies})
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_package()
