include(${CMAKE_SOURCE_DIR}/cmake/pr_common.cmake)

set(PROJ_NAME oskit)
pr_add_library(${PROJ_NAME} SHARED)

if(UNIX)
    option(USE_LIBNOTIFY "Use libnotify for notifications (if OFF, uses sdbus)" ON)

    if(USE_LIBNOTIFY)
        message(STATUS "Notifications: Using libnotify")

        find_package(PkgConfig REQUIRED)
        pkg_check_modules(LIBNOTIFY REQUIRED libnotify)

        target_include_directories(${PROJ_NAME} PRIVATE ${LIBNOTIFY_INCLUDE_DIRS})
        target_link_libraries(${PROJ_NAME} PRIVATE ${LIBNOTIFY_LIBRARIES})
        # target_compile_options(${PROJ_NAME} PRIVATE ${LIBNOTIFY_CFLAGS_OTHER})

        target_compile_definitions(${PROJ_NAME} PRIVATE USE_LIBNOTIFY)
    else()
        message(STATUS "Notifications: Using sdbus-c++")
        pr_add_external_dependency(${PROJ_NAME} sdbus_cpp LIBRARY)
        target_compile_definitions(${PROJ_NAME} PRIVATE USE_SDBUS)
    endif()
else()
	pr_add_dependency(${PROJ_NAME} sharedutils TARGET)

    pr_add_external_dependency(${PROJ_NAME} wintoast LIBRARY)
endif()

pr_init_module(${PROJ_NAME})

pr_add_compile_definitions(${PROJ_NAME} -DPOSKIT_EXPORT)

pr_finalize(${PROJ_NAME})
