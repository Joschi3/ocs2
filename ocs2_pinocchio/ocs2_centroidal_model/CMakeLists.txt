cmake_minimum_required(VERSION 3.14)
project(ocs2_centroidal_model)

find_package(ament_cmake REQUIRED)

# ament/ROS packages
find_package(ocs2_core REQUIRED)
find_package(ocs2_robotic_tools REQUIRED)
find_package(ocs2_robotic_assets REQUIRED)
find_package(ocs2_pinocchio_interface REQUIRED)
find_package(urdf REQUIRED)

# system libs
find_package(urdfdom REQUIRED)
find_package(hpp-fcl REQUIRED)
find_package(pinocchio REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(Boost REQUIRED COMPONENTS system filesystem log_setup log)

# hpp-fcl target name can vary
set(HPPFCL_TARGET "")
if(TARGET hpp-fcl)
  set(HPPFCL_TARGET hpp-fcl)
elseif(TARGET hpp::fcl)
  set(HPPFCL_TARGET hpp::fcl)
elseif(DEFINED HPP_FCL_LIBRARIES)
  set(HPPFCL_TARGET ${HPP_FCL_LIBRARIES})
else()
  set(HPPFCL_TARGET hpp-fcl)
endif()

set(URDFDOM_LIBS urdfdom_model urdfdom_world urdfdom_sensor)

set(FLAGS
  ${OCS2_CXX_FLAGS}
  ${pinocchio_CFLAGS_OTHER}
  -Wno-ignored-attributes
  -Wno-invalid-partial-specialization
  -DPINOCCHIO_URDFDOM_TYPEDEF_SHARED_PTR
  -DPINOCCHIO_URDFDOM_USE_STD_SHARED_PTR
)

include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
  src/PinocchioCentroidalDynamics.cpp
  src/PinocchioCentroidalDynamicsAD.cpp
  src/CentroidalModelRbdConversions.cpp
  src/CentroidalModelPinocchioMapping.cpp
  src/CentroidalModelInfo.cpp
  src/FactoryFunctions.cpp
  src/ModelHelperFunctions.cpp
)

# ament deps (calls target_link_libraries in PLAIN mode)
ament_target_dependencies(${PROJECT_NAME}
  ocs2_core
  ocs2_robotic_tools
  ocs2_robotic_assets
  ocs2_pinocchio_interface
  urdf
)

# Link non-ament/system deps in PLAIN mode too (no PUBLIC/PRIVATE keywords)
target_link_libraries(${PROJECT_NAME}
  pinocchio::pinocchio
  ${HPPFCL_TARGET}
  ${URDFDOM_LIBS}
  Eigen3::Eigen
  Boost::filesystem
  Boost::system
  Boost::log_setup
  Boost::log
)

target_compile_options(${PROJECT_NAME} PUBLIC ${FLAGS})
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include/${PROJECT_NAME}
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

include(CTest)
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(${PROJECT_NAME}_test
    test/testAnymalCentroidalModel.cpp
  )
  target_include_directories(${PROJECT_NAME}_test PRIVATE
    test/include
  )
  # Also PLAIN signature here
  target_link_libraries(${PROJECT_NAME}_test
    ${PROJECT_NAME}
    pinocchio::pinocchio
    ${HPPFCL_TARGET}
    ${URDFDOM_LIBS}
    Eigen3::Eigen
    Boost::filesystem
    Boost::system
    Boost::log_setup
    Boost::log
  )
  target_compile_options(${PROJECT_NAME}_test PUBLIC ${FLAGS})
endif()

ament_export_dependencies(
  ocs2_core
  ocs2_robotic_tools
  ocs2_robotic_assets
  ocs2_pinocchio_interface
  urdf
)
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_package(CONFIG_EXTRAS "cmake/pinocchio_config.cmake")
