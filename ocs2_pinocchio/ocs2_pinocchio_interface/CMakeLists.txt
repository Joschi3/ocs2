cmake_minimum_required(VERSION 3.14)
project(ocs2_pinocchio_interface)

set(dependencies
  ocs2_core
  ocs2_robotic_tools
  coal
  pinocchio
  urdf
  urdfdom
)

find_package(ament_cmake REQUIRED)
find_package(ocs2_core REQUIRED)
find_package(ocs2_robotic_tools REQUIRED)
find_package(urdf REQUIRED)
find_package(urdfdom REQUIRED)
find_package(urdfdom_headers REQUIRED)

find_package(coal REQUIRED)
find_package(pinocchio REQUIRED)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(Boost REQUIRED COMPONENTS
  filesystem
  log_setup
  log
)

# Add pinocchio configurations
include(cmake/pinocchio_config.cmake)

###########
## Build ##
###########

include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
  ${pinocchio_INCLUDE_DIRS}
)

set(dependencies_targets
  ${ocs2_core_TARGETS}
  ${ocs2_robotic_tools_TARGETS}
  coal::coal
  pinocchio::pinocchio
  ${urdf_TARGETS}
  urdfdom::urdfdom_model
  urdfdom::urdfdom_world
  urdfdom::urdfdom_sensor
)

# ocs2 pinocchio interface library
add_library(${PROJECT_NAME} SHARED
  src/PinocchioInterface.cpp
  src/PinocchioInterfaceCppAd.cpp
  src/PinocchioEndEffectorKinematics.cpp
  src/PinocchioEndEffectorKinematicsCppAd.cpp
  src/urdf.cpp
)
target_link_libraries(${PROJECT_NAME} ${dependencies_targets})
target_compile_options(${PROJECT_NAME} PUBLIC ${OCS2_PINOCCHIO_FLAGS})

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


############
# Testing ##
############
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(testPinocchioInterface
    test/testPinocchioInterface.cpp
    test/testPinocchioEndEffectorKinematics.cpp
  )
  target_link_libraries(testPinocchioInterface
    ${PROJECT_NAME}
  )
  target_link_libraries(testPinocchioInterface ${dependencies_targets})
  target_compile_options(testPinocchioInterface PUBLIC ${OCS2_PINOCCHIO_FLAGS})
endif()

# Propagate dependencies
ament_export_dependencies(${dependencies})
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)

ament_package(CONFIG_EXTRAS "cmake/pinocchio_config.cmake")
