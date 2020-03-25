include_directories(${PROJECT_ROOT}/src/include)
include_directories(${PROJECT_ROOT}/src/lib)

add_custom_target(testsuite_build
  COMMAND PREFIX=${CMAKE_CURRENT_LIST_DIR}/testsuite ${CMAKE_CURRENT_LIST_DIR}/testsuite/generate_all.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
)

#add_custom_target(testsuite
#  DEPENDS testsuite_build
#)