include(ExternalProject)
ExternalProject_add(wabt
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/wabt
  GIT_REPOSITORY "https://github.com/WasmVM/wabt.git"
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/wabt
  GIT_PROGRESS TRUE
  GIT_SHALLOW TRUE
  GIT_TAG main
  INSTALL_COMMAND cmake -E echo "Skipping install step."
)