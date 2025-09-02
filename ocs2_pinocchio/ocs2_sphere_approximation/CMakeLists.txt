cmake_minimum_required(VERSION 3.5)
project(ocs2_sphere_approximation)
set(CMAKE_CXX_STANDARD 17)

find_package(ament_cmake REQUIRED)
find_package(Boost REQUIRED COMPONENTS log_setup log filesystem system)

set(ament_deps
  ocs2_core
  ocs2_robotic_tools
  ocs2_pinocchio_interface
  ocs2_robotic_assets
  urdf
)

foreach(pkg IN LISTS ament_deps)
  find_package(${pkg} REQUIRED)
endforeach()

find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(pinocchio REQUIRED)        # pinocchio::pinocchio
find_package(hpp-fcl REQUIRED)          # hpp-fcl::hpp-fcl
find_package(tinyxml2 REQUIRED)         # tinyxml2::tinyxml2
find_package(urdfdom_headers REQUIRED)

# ---- Build flags ----
set(FLAGS
  ${OCS2_CXX_FLAGS}
  -DPINOCCHIO_WITH_HPP_FCL
  -Wno-ignored-attributes
  -Wno-invalid-partial_specialization
  -DPINOCCHIO_URDFDOM_TYPEDEF_SHARED_PTR
  -DPINOCCHIO_URDFDOM_USE_STD_SHARED_PTR
)

include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
)

# ---- Library ----
add_library(${PROJECT_NAME}
  src/SphereApproximation.cpp
  src/PinocchioSphereInterface.cpp
  src/PinocchioSphereKinematics.cpp
  src/PinocchioSphereKinematicsCppAd.cpp
)
ament_target_dependencies(${PROJECT_NAME} ${ament_deps})
target_compile_options(${PROJECT_NAME} PUBLIC ${FLAGS})
target_link_libraries(${PROJECT_NAME}
  pinocchio::pinocchio
  hpp-fcl::hpp-fcl
  tinyxml2::tinyxml2
)

# ---- Clang tooling (optional) ----
find_package(cmake_clang_tools QUIET)
if(cmake_clang_tools_FOUND)
  message(STATUS "Run clang tooling")
  add_clang_tooling(
    TARGETS ${PROJECT_NAME}
    SOURCE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include
    CT_HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    CF_WERROR
  )
endif()

# ---- Install ----
install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include/${PROJECT_NAME}
)
install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

# ---- Tests ----
find_package(ament_lint_auto REQUIRED)
ament_lint_auto_find_test_dependencies()
find_package(ament_cmake_gtest REQUIRED)

ament_add_gtest(PinocchioSphereKinematicsTest
  test/testPinocchioSphereKinematics.cpp
)
ament_target_dependencies(PinocchioSphereKinematicsTest ${ament_deps})
target_compile_options(PinocchioSphereKinematicsTest PUBLIC ${FLAGS})
target_link_libraries(PinocchioSphereKinematicsTest
  ${PROJECT_NAME}
  tinyxml2::tinyxml2
)

# ---- Export ----
ament_export_dependencies(${ament_deps})
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_package()
