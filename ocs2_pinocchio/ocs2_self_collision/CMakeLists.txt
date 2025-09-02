cmake_minimum_required(VERSION 3.5)
project(ocs2_self_collision)
set(CMAKE_CXX_STANDARD 17)

# -----------------------------
# ament / ROS dependencies
# -----------------------------
set(ament_deps
  ocs2_core
  ocs2_robotic_tools
  ocs2_pinocchio_interface
)

find_package(ament_cmake REQUIRED)
foreach(dep IN LISTS ament_deps)
  find_package(${dep} REQUIRED)
endforeach()

# -----------------------------
# Third-party (non-ament) deps
# -----------------------------
find_package(Boost REQUIRED COMPONENTS system filesystem log_setup log)
find_package(pinocchio REQUIRED)
find_package(hpp-fcl  REQUIRED)
find_package(Eigen3   3.3 REQUIRED NO_MODULE)
find_package(tinyxml2 REQUIRED)

# Robust target name for tinyxml2 (distros differ)
if(TARGET tinyxml2::tinyxml2)
  set(TINYXML2_TARGET tinyxml2::tinyxml2)
elseif(TARGET TinyXML2::TinyXML2)
  set(TINYXML2_TARGET TinyXML2::TinyXML2)
else()
  message(FATAL_ERROR "tinyxml2 CMake target not found")
endif()

# -----------------------------
# Compile flags / defines
# -----------------------------
set(FLAGS
  -Wno-ignored-attributes
  -Wno-invalid-partial-specialization   # silence warning with unsupported Eigen Tensor
)
set(DEFS
  PINOCCHIO_WITH_HPP_FCL
  PINOCCHIO_URDFDOM_TYPEDEF_SHARED_PTR
  PINOCCHIO_URDFDOM_USE_STD_SHARED_PTR
)

# -----------------------------
# Library (build shared to avoid DSO issues downstream)
# -----------------------------
add_library(${PROJECT_NAME} SHARED
  src/PinocchioGeometryInterface.cpp
  src/SelfCollision.cpp
  src/SelfCollisionCppAd.cpp
  src/SelfCollisionConstraint.cpp
  src/SelfCollisionConstraintCppAd.cpp
)

# Public includes for consumers
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_compile_options(${PROJECT_NAME} PUBLIC ${FLAGS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${DEFS})

# ament packages (this call uses PLAIN signature; keep all tll calls PLAIN)
ament_target_dependencies(${PROJECT_NAME} ${ament_deps})

# Link third-party imported targets (PLAIN signature to avoid mixing with ament)
target_link_libraries(${PROJECT_NAME}
  pinocchio::pinocchio
  hpp-fcl::hpp-fcl
  Eigen3::Eigen
  Boost::system
  Boost::filesystem
  Boost::log_setup
  Boost::log
  ${TINYXML2_TARGET} 
)

# -----------------------------
# Optional clang tooling
# -----------------------------
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

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_include_directories("include/${PROJECT_NAME}")
# Export ament deps plus 3rd-party so downstream find_package works 
ament_export_dependencies(
  ${ament_deps}
  pinocchio
  hpp-fcl
  Eigen3
  Boost
  tinyxml2
)

ament_package()
