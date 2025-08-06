include(${CMAKE_SOURCE_DIR}/cmake/pr_common.cmake)

set(PROJ_NAME oskit)
pr_add_library(${PROJ_NAME} SHARED)

if(UNIX)
    pr_add_external_dependency(${PROJ_NAME} sdbus_cpp LIBRARY)
endif()

pr_init_module(${PROJ_NAME})

pr_finalize(${PROJ_NAME})
