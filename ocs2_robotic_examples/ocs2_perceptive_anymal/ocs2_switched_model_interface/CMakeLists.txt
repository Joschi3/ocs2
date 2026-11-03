cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(ocs2_switched_model_interface)

## ament_cmake Dependencies
set(dependencies
  rclcpp
  ocs2_core
  ocs2_msgs
  ocs2_oc
  ocs2_ros_interfaces
  ocs2_robotic_tools
  ocs2_switched_model_msgs
)
find_package(ament_cmake REQUIRED)
find_package(ocs2_core REQUIRED)
find_package(ocs2_msgs REQUIRED)
find_package(ocs2_oc REQUIRED)
find_package(ocs2_ros_interfaces REQUIRED)
find_package(ocs2_robotic_tools REQUIRED)
find_package(ocs2_switched_model_msgs REQUIRED)
find_package(rclcpp REQUIRED)

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
)

set(dependencies_targets
  ${rclcpp_TARGETS}
  ${ocs2_core_TARGETS}
  ${ocs2_msgs_TARGETS}
  ${ocs2_oc_TARGETS}
  ${ocs2_ros_interfaces_TARGETS}
  ${ocs2_robotic_tools_TARGETS}
  ${ocs2_switched_model_msgs_TARGETS}
)

add_library(${PROJECT_NAME}
	src/analytical_inverse_kinematics/AnalyticalInverseKinematics.cpp
	src/analytical_inverse_kinematics/LegInverseKinematicParameters.cpp
	src/constraint/EndEffectorVelocityConstraint.cpp
	src/constraint/FootNormalConstraint.cpp
	src/constraint/FrictionConeConstraint.cpp
	src/constraint/ZeroForceConstraint.cpp
	src/dynamics/ComKinoSystemDynamicsAd.cpp
	src/dynamics/ComKinoDynamicsParameters.cpp
	src/cost/CollisionAvoidanceCost.cpp
	src/cost/FootPlacementCost.cpp
	src/cost/FrictionConeCost.cpp
	src/cost/LinearStateInequalitySoftConstraint.cpp
	src/cost/MotionTrackingCost.cpp
	src/cost/MotionTrackingTerminalCost.cpp
	src/cost/TorqueLimitsSoftConstraint.cpp
	src/foot_planner/KinematicFootPlacementPenalty.cpp
	src/foot_planner/CubicSpline.cpp
	src/foot_planner/FootPhase.cpp
	src/foot_planner/QuinticSplineSwing.cpp
	src/foot_planner/SplineCpg.cpp
	src/foot_planner/SwingSpline3d.cpp
	src/foot_planner/SwingTrajectoryPlanner.cpp
	src/initialization/ComKinoInitializer.cpp
	src/logic/Gait.cpp
	src/logic/GaitAdaptation.cpp
	src/logic/GaitReceiver.cpp
	src/logic/GaitSchedule.cpp
	src/logic/ModeSequenceTemplate.cpp
	src/logic/SingleLegLogic.cpp
	src/logic/DynamicsParametersSynchronizedModule.cpp
	src/logic/SwitchedModelModeScheduleManager.cpp
	src/ros_msg_conversions/RosMsgConversions.cpp
	src/terrain/PlanarSignedDistanceField.cpp
	src/terrain/PlanarTerrainModel.cpp
	src/terrain/PlaneFitting.cpp
	src/terrain/TerrainPlane.cpp
	src/core/ComModelBase.cpp
	src/core/KinematicsModelBase.cpp
	src/core/ModelSettings.cpp
	src/core/SwitchedModelPrecomputation.cpp
	src/core/TorqueApproximation.cpp
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


#############
## Testing ##
#############
find_package(ament_lint_auto REQUIRED)
ament_lint_auto_find_test_dependencies()
find_package(ament_cmake_gtest)

ament_add_gtest(test_constraints
	test/constraint/testZeroForceConstraint.cpp
	test/constraint/testFrictionConeConstraint.cpp
)
target_link_libraries(test_constraints ${dependencies_targets})
target_link_libraries(test_constraints
	${PROJECT_NAME}
)

ament_add_gtest(test_${PROJECT_NAME}_core
	test/core/testRotation.cpp
)
target_link_libraries(test_${PROJECT_NAME}_core ${dependencies_targets})
target_link_libraries(test_${PROJECT_NAME}_core
	${PROJECT_NAME}
)

ament_add_gtest(test_${PROJECT_NAME}_cost
	test/cost/testFootplacementCost.cpp
	test/cost/testFrictionConeCost.cpp
	test/cost/testTorqueLimitsSoftConstraint.cpp
)
target_link_libraries(test_${PROJECT_NAME}_cost ${dependencies_targets})
target_link_libraries(test_${PROJECT_NAME}_cost
	${PROJECT_NAME}
)

ament_add_gtest(test_${PROJECT_NAME}_footplanner
	test/foot_planner/testSwingPhase.cpp
)
target_link_libraries(test_${PROJECT_NAME}_footplanner ${dependencies_targets})
target_link_libraries(test_${PROJECT_NAME}_footplanner
	${PROJECT_NAME}
)

ament_add_gtest(test_${PROJECT_NAME}_logic
	test/logic/testEarlyTouchDown.cpp
	test/logic/testExtractContractTimings.cpp
	test/logic/testGait.cpp
	test/logic/testGaitSwitching.cpp
	test/logic/testGaitSchedule.cpp
	test/logic/testSingleLegLogic.cpp
)
target_link_libraries(test_${PROJECT_NAME}_logic ${dependencies_targets})
target_link_libraries(test_${PROJECT_NAME}_logic
	${PROJECT_NAME}
)

ament_add_gtest(test_${PROJECT_NAME}_terrain
	test/terrain/testTerrainPlane.cpp
)
target_link_libraries(test_${PROJECT_NAME}_terrain ${dependencies_targets})
target_link_libraries(test_${PROJECT_NAME}_terrain
	${PROJECT_NAME}
)

ament_add_gtest(test_${PROJECT_NAME}_convexTerrain
	test/terrain/testConvexTerrain.cpp
)
target_link_libraries(test_${PROJECT_NAME}_convexTerrain ${dependencies_targets})
target_link_libraries(test_${PROJECT_NAME}_convexTerrain
	${PROJECT_NAME}
)

ament_export_dependencies(${dependencies})
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_package()
