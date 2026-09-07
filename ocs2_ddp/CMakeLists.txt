cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(ocs2_ddp)

find_package(ament_cmake REQUIRED)
find_package(ocs2_core REQUIRED)
find_package(ocs2_mpc REQUIRED)
find_package(ocs2_qp_solver REQUIRED)
find_package(Boost REQUIRED COMPONENTS
  filesystem
  log_setup
  log
)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)




###################################
## ament_cmake specific configuration ##
###################################

set(dependencies
    ocs2_core
    ocs2_oc
    ocs2_mpc
    ocs2_qp_solver
)

###########
## Build ##
###########

include_directories(
  include
  test/include
  ${EIGEN3_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

set(dependencies_targets
  ${ocs2_core_TARGETS}
  ${ocs2_oc_TARGETS}
  ${ocs2_mpc_TARGETS}
  ${ocs2_qp_solver_TARGETS}
)

add_library(${PROJECT_NAME}
  src/riccati_equations/ContinuousTimeRiccatiEquations.cpp
  src/riccati_equations/DiscreteTimeRiccatiEquations.cpp
  src/riccati_equations/RiccatiModification.cpp
  src/search_strategy/LevenbergMarquardtStrategy.cpp
  src/search_strategy/LineSearchStrategy.cpp
  src/search_strategy/StrategySettings.cpp
  src/ContinuousTimeLqr.cpp
  src/GaussNewtonDDP.cpp
  src/HessianCorrection.cpp
  src/ILQR.cpp
  src/SLQ.cpp
  src/DDP_Settings.cpp
  src/DDP_HelperFunctions.cpp
)
target_link_libraries(${PROJECT_NAME} ${dependencies_targets})
target_compile_options(${PROJECT_NAME} PUBLIC ${OCS2_CXX_FLAGS})

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
install(DIRECTORY test/include/
  DESTINATION include/${PROJECT_NAME}
)

#############
## Testing ##
#############
find_package(ament_lint_auto REQUIRED)
ament_lint_auto_find_test_dependencies()
find_package(ament_cmake_gtest REQUIRED)

ament_add_gtest(testLineSearchConcurrency test/testLineSearchConcurrency.cpp)
target_link_libraries(testLineSearchConcurrency ${PROJECT_NAME})

ament_add_gtest(exp0_ddp_test test/Exp0Test.cpp)
target_link_libraries(exp0_ddp_test ${PROJECT_NAME})
target_link_libraries(exp0_ddp_test ${dependencies_targets})

ament_add_gtest(exp1_ddp_test
  test/Exp1Test.cpp
)
target_link_libraries(exp1_ddp_test
  ${PROJECT_NAME}
)
target_link_libraries(exp1_ddp_test ${dependencies_targets})

ament_add_gtest(correctness_test
  test/CorrectnessTest.cpp
)
target_link_libraries(correctness_test
  ${PROJECT_NAME}
)
target_link_libraries(correctness_test ${dependencies_targets})

ament_add_gtest(riccati_ode_test
  test/RiccatiTest.cpp
)
target_link_libraries(riccati_ode_test
  ${PROJECT_NAME}
)
target_link_libraries(riccati_ode_test ${dependencies_targets})

ament_add_gtest(circular_kinematics_ddp_test
  test/CircularKinematicsTest.cpp
)
target_link_libraries(circular_kinematics_ddp_test
  ${PROJECT_NAME}
)
target_link_libraries(circular_kinematics_ddp_test ${dependencies_targets})

ament_add_gtest(hybrid_slq_test
  test/HybridSlqTest.cpp
)
target_link_libraries(hybrid_slq_test
  ${PROJECT_NAME}
)
target_link_libraries(hybrid_slq_test ${dependencies_targets})

ament_add_gtest(bouncing_mass_test
  test/bouncingmass/BouncingMassTest.cpp
  test/bouncingmass/OverallReference.cpp
  test/bouncingmass/Reference.cpp
)
target_link_libraries(bouncing_mass_test
  ${PROJECT_NAME}
)
target_link_libraries(bouncing_mass_test ${dependencies_targets})

ament_add_gtest(testContinuousTimeLqr
  test/testContinuousTimeLqr.cpp
)
target_link_libraries(testContinuousTimeLqr
  ${PROJECT_NAME}
)
target_link_libraries(testContinuousTimeLqr ${dependencies_targets})

ament_add_gtest(testDdpHelperFunction
  test/testDdpHelperFunction.cpp
)
target_link_libraries(testDdpHelperFunction
  ${PROJECT_NAME}
)
target_link_libraries(testDdpHelperFunction ${dependencies_targets})

ament_add_gtest(testReachingTask
  test/testReachingTask.cpp
)
target_link_libraries(testReachingTask
  ${PROJECT_NAME}
)
target_link_libraries(testReachingTask ${dependencies_targets})

ament_export_dependencies(${dependencies})
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_package()
