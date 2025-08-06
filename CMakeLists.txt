include(${CMAKE_SOURCE_DIR}/cmake/pr_common.cmake)

set(PROJ_NAME oskit)
pr_add_library(${PROJ_NAME} SHARED)

if(UNIX)
    pr_add_external_dependency(${PROJ_NAME} sdbus_cpp LIBRARY)
else()
	pr_add_dependency(${PROJ_NAME} sharedutils TARGET)

    pr_add_external_dependency(${PROJ_NAME} wintoast LIBRARY)
endif()

pr_init_module(${PROJ_NAME})

pr_add_compile_definitions(${PROJ_NAME} -DPOSKIT_EXPORT)

pr_finalize(${PROJ_NAME})
