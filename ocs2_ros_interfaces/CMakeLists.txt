cmake_minimum_required(VERSION 3.10)
project(ocs2_ros_interfaces)

# find_package(ament_cmake_auto REQUIRED)
# ament_auto_find_build_dependencies()

## Find ament_cmake macros and libraries
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ocs2_msgs REQUIRED)
find_package(ocs2_mpc REQUIRED)
find_package(std_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(interactive_markers REQUIRED)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(Boost REQUIRED COMPONENTS
  filesystem
  log_setup
  log
)




set(dependencies
  rclcpp
  ocs2_msgs
  ocs2_core
  ocs2_mpc
  std_msgs
  visualization_msgs
  geometry_msgs
  interactive_markers
)

###########
## Build ##
###########

## Specify additional locations of header files
include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

set(dependencies_targets
  ${rclcpp_TARGETS}
  ${ocs2_msgs_TARGETS}
  ${ocs2_core_TARGETS}
  ${ocs2_mpc_TARGETS}
  ${std_msgs_TARGETS}
  ${visualization_msgs_TARGETS}
  ${geometry_msgs_TARGETS}
  ${interactive_markers_TARGETS}
)

add_library(${PROJECT_NAME}
  src/command/TargetTrajectoriesRosPublisher.cpp
  src/command/TargetTrajectoriesInteractiveMarker.cpp
  src/command/TargetTrajectoriesKeyboardPublisher.cpp
  src/common/RosMsgConversions.cpp
  src/common/RosMsgHelpers.cpp
  src/mpc/MPC_ROS_Interface.cpp
  src/mrt/LoopshapingDummyObserver.cpp
  src/mrt/MRT_ROS_Dummy_Loop.cpp
  src/mrt/MRT_ROS_Interface.cpp
  src/synchronized_module/RosReferenceManager.cpp
  src/synchronized_module/SolverObserverRosCallbacks.cpp
  src/visualization/VisualizationHelpers.cpp
  src/visualization/VisualizationColors.cpp
)
target_link_libraries(${PROJECT_NAME} ${dependencies_targets})
target_compile_options(${PROJECT_NAME} PUBLIC ${OCS2_CXX_FLAGS})

add_executable(test_custom_callback_queue
  test/test_custom_callback_queue.cpp
)
target_link_libraries(test_custom_callback_queue ${dependencies_targets})
target_link_libraries(test_custom_callback_queue
  ${PROJECT_NAME}
)
target_compile_options(test_custom_callback_queue PRIVATE ${OCS2_CXX_FLAGS})

# multiplot remap node
add_executable(multiplot_remap
  src/multiplot/MultiplotRemap.cpp
)
target_link_libraries(multiplot_remap ${dependencies_targets})
target_link_libraries(test_custom_callback_queue
  ${PROJECT_NAME}
)
target_compile_options(multiplot_remap PRIVATE ${OCS2_CXX_FLAGS})

add_executable(${PROJECT_NAME}_lintTarget
  src/lintTarget.cpp
)
target_link_libraries(${PROJECT_NAME}_lintTarget ${dependencies_targets})
target_link_libraries(${PROJECT_NAME}_lintTarget
  ${PROJECT_NAME}
)


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
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
install(TARGETS multiplot_remap DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch multiplot
  DESTINATION include/${PROJECT_NAME}/
)

#############
## Testing ##
#############

ament_export_dependencies(${dependencies})
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_package()
# ament_auto_package()
