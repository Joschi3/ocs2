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
# Install & build dirs
# -----------------------
set(HPIPM_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH "HPIPM install path")
set(HPIPM_DOWNLOAD_DIR   "${CMAKE_CURRENT_BINARY_DIR}/download")
set(HPIPM_BUILD_DIR      "${CMAKE_CURRENT_BINARY_DIR}/build")

# -----------------------
# CPU target selection
# -----------------------
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|X86_64|amd64|AMD64)$")
  set(HPIPM_TARGET   "AVX"            CACHE STRING "" FORCE)
  set(BLASFEO_TARGET "X64_INTEL_HASWELL" CACHE STRING "" FORCE)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  set(HPIPM_TARGET   "ARMV8A"         CACHE STRING "" FORCE)
  set(BLASFEO_TARGET "ARMV8A"         CACHE STRING "" FORCE)
else()
  set(HPIPM_TARGET   "GENERIC"        CACHE STRING "" FORCE)
  set(BLASFEO_TARGET "GENERIC"        CACHE STRING "" FORCE)
endif()

# BLASFEO location (ament-installed under the same prefix)
set(BLASFEO_PATH         "${CMAKE_INSTALL_PREFIX}" CACHE PATH "" FORCE)
set(BLASFEO_INCLUDE_DIR  "${BLASFEO_PATH}/include" CACHE PATH "" FORCE)

# Prefer shared if HPIPM honors BUILD_SHARED_LIBS; don’t rely on it always
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(HPIPM_TESTING    OFF CACHE BOOL "" FORCE)

message(STATUS "HPIPM_TARGET=${HPIPM_TARGET}")
message(STATUS "BLASFEO_TARGET=${BLASFEO_TARGET}")
message(STATUS "BLASFEO_PATH=${BLASFEO_PATH}")

# -----------------------
# Fetch & configure HPIPM
# -----------------------
FetchContent_Declare(hpipm
  GIT_REPOSITORY https://github.com/giaf/hpipm
  GIT_TAG        255ffdf38d3a5e2c3285b29568ce65ae286e5faf
  SOURCE_DIR     ${HPIPM_DOWNLOAD_DIR}
  BINARY_DIR     ${HPIPM_BUILD_DIR}
)

# Pass options to subproject BEFORE MakeAvailable
set(FETCHCONTENT_QUIET OFF)

# Subproject cache args some forks honor
set(CMAKE_CACHE_ARGS
  -DHPIPM_TARGET:STRING=${HPIPM_TARGET}
  -DBLASFEO_TARGET:STRING=${BLASFEO_TARGET}
  -DBLASFEO_PATH:PATH=${BLASFEO_PATH}
  -DBLASFEO_INCLUDE_DIR:PATH=${BLASFEO_INCLUDE_DIR}
  -DBUILD_SHARED_LIBS:BOOL=ON
  -DHPIPM_TESTING:BOOL=OFF
)

FetchContent_MakeAvailable(hpipm)

# DO NOT redirect hpipm’s output into the install prefix.
# Instead, install it properly (whether it’s shared or static).
if(TARGET hpipm)
  # If the subproject already has install() rules, this is harmless.
  # If it doesn't, these lines ensure we install the built library.
  get_target_property(_hpipm_type hpipm TYPE)
  if(_hpipm_type STREQUAL "STATIC_LIBRARY")
    install(TARGETS hpipm ARCHIVE DESTINATION lib)
  elseif(_hpipm_type STREQUAL "SHARED_LIBRARY" OR _hpipm_type STREQUAL "MODULE_LIBRARY")
    install(TARGETS hpipm LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
  else()
    # Fallback: try both kinds
    install(TARGETS hpipm ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
  endif()
endif()

# Install headers from the subproject’s include tree
if(EXISTS "${HPIPM_DOWNLOAD_DIR}/include")
  install(DIRECTORY "${HPIPM_DOWNLOAD_DIR}/include/" DESTINATION include)
endif()

# -----------------------
# Our wrapper library
# -----------------------
include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
  # Pull HPIPM headers from the staged include after install,
  # but for build we include directly from the downloaded tree:
  ${HPIPM_DOWNLOAD_DIR}/include
)

set(dependencies
  blasfeo_ament
  ocs2_core
  ocs2_oc
  ocs2_qp_solver
  Eigen3
)

add_library(${PROJECT_NAME}
  src/HpipmInterface.cpp
  src/HpipmInterfaceSettings.cpp
)

ament_target_dependencies(${PROJECT_NAME} ${dependencies})

# Link against hpipm if the target exists; otherwise fallback to -lhpipm
if(TARGET hpipm)
  target_link_libraries(${PROJECT_NAME} hpipm)
else()
  target_link_libraries(${PROJECT_NAME} hpipm)
endif()

target_compile_options(${PROJECT_NAME} PUBLIC ${OCS2_CXX_FLAGS})

# -----------------------
# Install our wrapper
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
# Tests (respect BUILD_TESTING)
# -----------------------
include(CTest)
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_${PROJECT_NAME}
    test/testHpipmInterface.cpp
  )
  ament_target_dependencies(test_${PROJECT_NAME} ${dependencies})
  target_link_libraries(test_${PROJECT_NAME}
    ${PROJECT_NAME}
    hpipm
  )
endif()

# -----------------------
# Export
# -----------------------
ament_export_dependencies(${dependencies} hpipm)
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_package()
