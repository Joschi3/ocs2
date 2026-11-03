cmake_minimum_required(VERSION 3.10)
project(ocs2_mobile_manipulator)
set(CMAKE_CXX_STANDARD 17)

# -----------------------------
# Dependencies
# -----------------------------
# ament/ROS packages only
set(ament_deps
  ocs2_core
  ocs2_ddp
  ocs2_mpc
  ocs2_robotic_tools
  ocs2_robotic_assets
  ocs2_pinocchio_interface
  ocs2_self_collision
)

find_package(ament_cmake REQUIRED)
foreach(dep IN LISTS ament_deps)
  find_package(${dep} REQUIRED)
endforeach()

# Third-party (non-ament) packages via CMake CONFIG
find_package(Boost REQUIRED COMPONENTS filesystem log_setup log)
find_package(coal REQUIRED)
find_package(pinocchio REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# -----------------------------
# Compile flags / defines
# -----------------------------
set(FLAGS
  -Wno-ignored-attributes
  -Wno-invalid-partial-specialization   # silence warning with unsupported Eigen Tensor
)
set(DEFS
  PINOCCHIO_URDFDOM_TYPEDEF_SHARED_PTR
  PINOCCHIO_URDFDOM_USE_STD_SHARED_PTR
)

# -----------------------------
# Generated header
# -----------------------------
configure_file(
  "${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/package_path.h.in"
  "${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}/package_path.h" @ONLY
)

set(ament_deps_targets
  ${ocs2_core_TARGETS}
  ${ocs2_ddp_TARGETS}
  ${ocs2_mpc_TARGETS}
  ${ocs2_robotic_tools_TARGETS}
  ${ocs2_robotic_assets_TARGETS}
  ${ocs2_pinocchio_interface_TARGETS}
  ${ocs2_self_collision_TARGETS}
)

# -----------------------------
# Library
# -----------------------------
add_library(${PROJECT_NAME}
  src/FactoryFunctions.cpp
  src/MobileManipulatorPreComputation.cpp
  src/MobileManipulatorPinocchioMapping.cpp
  src/constraint/EndEffectorConstraint.cpp
  src/dynamics/WheelBasedMobileManipulatorDynamics.cpp
  src/dynamics/DefaultManipulatorDynamics.cpp
  src/dynamics/FloatingArmManipulatorDynamics.cpp
  src/dynamics/FullyActuatedFloatingArmManipulatorDynamics.cpp
  src/MobileManipulatorInterface.cpp
)

# Public includes for consumers of this library
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_compile_options(${PROJECT_NAME} PUBLIC ${FLAGS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${DEFS})

# ament deps (PLAIN signature; keep all tll calls PLAIN)
target_link_libraries(${PROJECT_NAME} ${ament_deps_targets})

# Link 3rd-party imported targets (PLAIN signature to avoid mixing)
target_link_libraries(${PROJECT_NAME}
  pinocchio::pinocchio
  coal::coal
  Eigen3::Eigen
  Boost::filesystem
  Boost::log_setup
  Boost::log
)


# -----------------------------
# Install / Export
# -----------------------------
install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include/${PROJECT_NAME}
)
install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(DIRECTORY config DESTINATION share/${PROJECT_NAME}/)
install(DIRECTORY auto_generated DESTINATION share/${PROJECT_NAME}/)


# -----------------------------
# Tests
# -----------------------------
find_package(ament_lint_auto REQUIRED)
ament_lint_auto_find_test_dependencies()
find_package(ament_cmake_gtest REQUIRED)

macro(add_ocs2_test APP_NAME APP_SRC)
  ament_add_gtest(${APP_NAME} ${APP_SRC} TIMEOUT 300)
  target_include_directories(${APP_NAME} PRIVATE ${PROJECT_BINARY_DIR}/include)
  target_link_libraries(${APP_NAME} ${ament_deps_targets})
  # Link third-party libs and the library under test (PLAIN signature)
  target_link_libraries(${APP_NAME}
    ${PROJECT_NAME}
    pinocchio::pinocchio
    coal::coal
    Eigen3::Eigen
    Boost::filesystem
    Boost::log_setup
    Boost::log
  )
endmacro()

add_ocs2_test(SelfCollisionTest           test/testSelfCollision.cpp)
add_ocs2_test(EndEffectorConstraintTest   test/testEndEffectorConstraint.cpp)
add_ocs2_test(DummyMobileManipulatorTest  test/testDummyMobileManipulator.cpp)
set_tests_properties(DummyMobileManipulatorTest PROPERTIES TIMEOUT 600)


ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_dependencies(${ament_deps})
ament_package()
