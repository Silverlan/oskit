include(${CMAKE_SOURCE_DIR}/cmake/pr_common.cmake)

set(PROJ_NAME oskit)
pr_add_library(${PROJ_NAME} SHARED)

pr_add_external_dependency(${PROJ_NAME} sdbus_cpp LIBRARY)

pr_init_module(${PROJ_NAME})

pr_finalize(${PROJ_NAME})
