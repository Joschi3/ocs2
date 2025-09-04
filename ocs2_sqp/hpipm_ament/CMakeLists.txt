cmake_minimum_required(VERSION 3.14)
project(hpipm_ament)

find_package(ament_cmake REQUIRED)
find_package(blasfeo_ament REQUIRED)
find_package(ocs2_core REQUIRED)
find_package(ocs2_oc REQUIRED)
find_package(ocs2_qp_solver REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem log_setup log)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

include(FetchContent)

# -----------------------
# Install & third-party paths
# -----------------------
# In your packaging rules, CMAKE_INSTALL_PREFIX is /opt/hector/jazzy.
set(HPIPM_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH "Install prefix for hpipm")
# BLASFEO is installed under the same prefix by blasfeo_ament.
set(BLASFEO_PATH "${CMAKE_INSTALL_PREFIX}" CACHE PATH "BLASFEO prefix for hpipm")

# -----------------------
# CPU target selection for hpipm
# -----------------------
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|X86_64|amd64|AMD64)$")
  set(HPIPM_TARGET "AVX")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  set(HPIPM_TARGET "ARMV8A")
else()
  set(HPIPM_TARGET "GENERIC")
endif()

# Choose shared or static for hpipm itself.
# If you want a .so, set this ON; if you prefer static, set OFF and link to the .a.
set(HPIPM_BUILD_SHARED ON)

message(STATUS "Configuring hpipm with TARGET=${HPIPM_TARGET}, BLASFEO_PATH=${BLASFEO_PATH}, BUILD_SHARED_LIBS=${HPIPM_BUILD_SHARED}")

# -----------------------
# Fetch & configure hpipm
# -----------------------
FetchContent_Declare(hpipm
  GIT_REPOSITORY https://github.com/giaf/hpipm
  GIT_TAG        255ffdf38d3a5e2c3285b29568ce65ae286e5faf
  SOURCE_DIR     ${CMAKE_CURRENT_BINARY_DIR}/hpipm-src
  BINARY_DIR     ${CMAKE_CURRENT_BINARY_DIR}/hpipm-build
  # CRUCIAL: push the right cache args into the sub-build
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${HPIPM_PREFIX}
    -DTARGET=${HPIPM_TARGET}
    -DBLASFEO_PATH=${BLASFEO_PATH}
    -DREF_BLAS=0
    -DBUILD_SHARED_LIBS=$<IF:$<BOOL:${HPIPM_BUILD_SHARED}>,ON,OFF>
    -DHPIPM_TESTING=OFF
)

FetchContent_MakeAvailable(hpipm)  # defines target hpipm

# -----------------------
# Our wrapper library
# -----------------------
add_library(${PROJECT_NAME}
  src/HpipmInterface.cpp
  src/HpipmInterfaceSettings.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${EIGEN3_INCLUDE_DIRS}
  # During the build, include hpipm headers from the fetched tree
  ${CMAKE_CURRENT_BINARY_DIR}/hpipm-src/include
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

ament_target_dependencies(${PROJECT_NAME}
  blasfeo_ament
  ocs2_core
  ocs2_oc
  ocs2_qp_solver
  Eigen3
)

# Link to the fetched hpipm target
target_link_libraries(${PROJECT_NAME} PUBLIC hpipm)

# Optional: your extra compile flags
target_compile_options(${PROJECT_NAME} PUBLIC ${OCS2_CXX_FLAGS})

# -----------------------
# Install our wrapper (hpipm installs itself via its own install() rules)
# -----------------------
install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include/${PROJECT_NAME}
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

# -----------------------
# Tests (optional)
# -----------------------
include(CTest)
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_${PROJECT_NAME} test/testHpipmInterface.cpp)
  ament_target_dependencies(test_${PROJECT_NAME}
    blasfeo_ament ocs2_core ocs2_oc ocs2_qp_solver Eigen3)
  target_link_libraries(test_${PROJECT_NAME} ${PROJECT_NAME} hpipm)
endif()

# -----------------------
# Export
# -----------------------
ament_export_dependencies(blasfeo_ament ocs2_core ocs2_oc ocs2_qp_solver Eigen3)
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_package()
