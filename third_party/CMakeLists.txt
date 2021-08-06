include(ExternalProject)
ExternalProject_add(wabt
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/wabt
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/wabt
  INSTALL_COMMAND cmake -E echo "Skipping install step."
)