cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project (ocs2_frank_wolfe)

find_package(ament_cmake REQUIRED)
find_package(ocs2_core REQUIRED)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

find_package(Boost REQUIRED COMPONENTS
  filesystem
  log_setup
  log
)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
find_package(GLPK MODULE REQUIRED)

###########
## Build ##
###########

## Include directories
include_directories(
    include
    ${EIGEN3_INCLUDE_DIRS}
)

set(dependencies
    ocs2_core
    GLPK
)

set(dependencies_targets
  ${ocs2_core_TARGETS}
  GLPK::GLPK
)

add_library(${PROJECT_NAME}
  src/FrankWolfeDescentDirection.cpp
  src/GradientDescent.cpp
)
target_link_libraries(${PROJECT_NAME} ${dependencies_targets})
target_compile_options(${PROJECT_NAME} PUBLIC ${OCS2_CXX_FLAGS})

#############
## Install ##
#############

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include/${PROJECT_NAME}
)

#############
## Testing ##
#############
find_package(ament_lint_auto REQUIRED)
ament_lint_auto_find_test_dependencies()
find_package(ament_cmake_gtest REQUIRED)

ament_add_gtest(quadratic_test
  test/QuadraticTest.cpp
)
target_link_libraries(quadratic_test ${dependencies_targets})
target_link_libraries(quadratic_test
  ${PROJECT_NAME}
  GLPK::GLPK
)

ament_add_gtest(matyas_test
  test/MatyasTest.cpp
)
target_link_libraries(matyas_test ${dependencies_targets})
target_link_libraries(matyas_test
  ${PROJECT_NAME}
  GLPK::GLPK
)

ament_add_gtest(glpk_test
  test/testGLPK.cpp
)
target_link_libraries(glpk_test ${dependencies_targets})
target_link_libraries(glpk_test
  ${PROJECT_NAME}
  GLPK::GLPK
)

ament_export_dependencies(${dependencies})
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)

ament_package()
