cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(ocs2_ocs2)

find_package(ament_cmake REQUIRED)
find_package(ocs2_core REQUIRED)
find_package(ocs2_ddp REQUIRED)
# find_package(ocs2_frank_wolfe REQUIRED)
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
    ocs2_ddp
    # ocs2_frank_wolfe
)

###########
## Build ##
###########

include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

set(dependencies_targets
  ${ocs2_core_TARGETS}
  ${ocs2_ddp_TARGETS}
)

add_executable(${PROJECT_NAME}_lintTarget
  src/lintTarget.cpp
)
target_link_libraries(${PROJECT_NAME}_lintTarget ${dependencies_targets})


#############
## Install ##
#############

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

#############
## Testing ##
#############


ament_export_dependencies(${dependencies})
ament_export_include_directories("include/${PROJECT_NAME}")
ament_package()
