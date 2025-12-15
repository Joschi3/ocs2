cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
project(ocs2_cartpole_ros)




set(dependencies
  rclcpp
  ocs2_core
  ocs2_ddp
  ocs2_mpc
  ocs2_ros_interfaces
  ocs2_robotic_tools
  ocs2_cartpole
  ocs2_robotic_assets
)

find_package(ament_cmake REQUIRED)
find_package(ocs2_core REQUIRED)
find_package(ocs2_ddp REQUIRED)
find_package(ocs2_mpc REQUIRED)
find_package(ocs2_robotic_tools REQUIRED)
find_package(ocs2_ros_interfaces REQUIRED)
find_package(ocs2_cartpole REQUIRED)
find_package(ocs2_robotic_assets REQUIRED)
find_package(rclcpp REQUIRED)

find_package(Boost REQUIRED COMPONENTS
  system
  filesystem
  log_setup
  log
)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

###########
## Build ##
###########

include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

# Mpc node
add_executable(cartpole_mpc
  src/CartpoleMpcNode.cpp
)
ament_target_dependencies(cartpole_mpc
  ${dependencies}
)

# Dummy node
add_executable(cartpole_dummy_test
  src/DummyCartpoleNode.cpp
  src/CartpoleDummyVisualization.cpp
)
ament_target_dependencies(cartpole_dummy_test
  ${dependencies}
)

#############
## Install ##
#############

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
install(
  TARGETS
    cartpole_mpc
    cartpole_dummy_test
    DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY launch rviz
  DESTINATION share/${PROJECT_NAME}/
)

ament_export_dependencies(${dependencies})
ament_export_include_directories("include/${PROJECT_NAME}")
ament_package()
