cmake_minimum_required(VERSION 3.21...3.24)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

# strongly encouraged to enable this globally to avoid conflicts between -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for
# example when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS ON)

# Add project_options https://github.com/aminya/project_options
include(cmake/CPM.cmake)
cpmaddpackage(
        NAME
        project_options
        GITHUB_REPOSITORY
        aminya/project_options
        VERSION           0.32.1
        #GIT_TAG
        #main
        DOWNLOAD_ONLY)
if(project_options_ADDED)
    include(${project_options_SOURCE_DIR}/Index.cmake)
endif()

option(ENABLE_CROSS_COMPILING "Detect cross compiler and setup toolchain" OFF)
if(ENABLE_CROSS_COMPILING)
    enable_cross_compiler()
endif()
run_vcpkg()

project(fixed_containers LANGUAGES CXX)
include(cmake/PreventInSourceBuilds.cmake)


# User options
include(cmake/CMakeOptions.cmake)
get_property(BUILDING_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(BUILDING_MULTI_CONFIG)
    if(NOT CMAKE_BUILD_TYPE)
        # Make sure that all supported configuration types have their associated conan packages available. You can reduce this list to only the
        # configuration types you use, but only if one is not forced-set on the command line for VS
        message(TRACE "Setting up multi-config build types")
        set(CMAKE_CONFIGURATION_TYPES
                Debug Release RelWithDebInfo MinSizeRel
                CACHE STRING "Enabled build types" FORCE)
    else()
        message(TRACE "User chose a specific build type, so we are using that")
        set(CMAKE_CONFIGURATION_TYPES
                ${CMAKE_BUILD_TYPE}
                CACHE STRING "Enabled build types" FORCE)
    endif()
endif()
include(cmake/ProjectOptions.cmake)

target_compile_options(project_options INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/WX /EHs->)
target_compile_options(project_options INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/wd4804 /wd4018>)  # due to magic-enum, mixed arithmetic
target_compile_options(project_options INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/wd4530>)  # due to ostream, exception handler

target_compile_options(project_options INTERFACE $<$<CXX_COMPILER_ID:Clang,GNU>:-fno-exceptions -ftemplate-backtrace-limit=0>)
target_compile_options(project_options INTERFACE $<$<CXX_COMPILER_ID:Clang,GNU>:-ftrivial-auto-var-init=pattern>)
#target_compile_options(project_options INTERFACE $<$<CXX_COMPILER_ID:Clang,GNU>:--gcc-install-dir=/usr/bin/../lib/gcc/x86_64-linux-gnu/11>)



target_compile_options(project_options INTERFACE $<$<CXX_COMPILER_ID:Clang>:
    # Only available in clang
    -Weverything
    # Disables C++98 to C++17 compatibility enforcement
    -Wno-c++98-compat-pedantic
    # Re-enable, as it is disabled by the previous one
    -Wc++98-compat-extra-semi

    # Has false positives
    # https://bugs.llvm.org/show_bug.cgi?id=18733
    # https://stackoverflow.com/questions/56041900/why-does-explicit-template-instantiation-result-in-weak-template-vtables-warning
    -Wno-weak-template-vtables
    #
    -Wno-global-constructors

    # Prevents iterators from returning non-references. See also:
    # https://quuxplusone.github.io/blog/2020/08/26/wrange-loop-analysis/
    -Wno-range-loop-bind-reference

    # due to gtest
    -Wno-covered-switch-default # v1.11.0 fails (needs new release). bazel points to a newer commit with the fix.
    -Wno-exit-time-destructors
    -Wno-used-but-marked-unused

    # This is failing on the `consteval` keyword for some reason
    -Wno-c++20-compat
    # Need stdlib uprev, as even std::vector triggers this
    -Wno-ctad-maybe-unsupported

    -Wno-padded
    -Wno-poison-system-directories
>)
target_compile_options(project_options INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4067>)
#target_compile_options(project_options INTERFACE $<$<CXX_COMPILER_ID:Clang,GNU>:-Wall -Wextra -Wpedantic>)


# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info
include(cmake/CPM.cmake)
# PackageProject.cmake will be used to make our target installable https://github.com/TheLartians/PackageProject.cmake
cpmaddpackage("gh:TheLartians/PackageProject.cmake@1.11.0")
# https://github.com/TheLartians/Format.cmake
cpmaddpackage(
        NAME
        Format.cmake
        VERSION
        1.7.3
        GITHUB_REPOSITORY
        TheLartians/Format.cmake
        OPTIONS
        # set to yes skip cmake formatting
        "FORMAT_SKIP_CMAKE NO"
        # path to exclude (optional, supports regular expressions)
        "CMAKE_FORMAT_EXCLUDE patches")


if(NOT TARGET magic_enum)
    find_package(magic_enum CONFIG REQUIRED)
endif()

add_library(fixed_containers INTERFACE)
add_library(fixed_containers::fixed_containers ALIAS fixed_containers)
target_include_directories(fixed_containers INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_system_libraries(fixed_containers INTERFACE magic_enum::magic_enum)


# TODO: split library in components like in bazel
#add_library(fixed_containers_bidirectional_iterator INTERFACE include/fixed_containers/bidirectional_iterator.hpp)
#add_library(fixed_containers::bidirectional_iterator ALIAS fixed_containers_bidirectional_iterator)
#target_include_directories(fixed_containers_bidirectional_iterator INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)


if(ENABLE_TESTING)
    # This variable is set by project() in CMake 3.21+
    string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}" PROJECT_IS_TOP_LEVEL)
    if(PROJECT_IS_TOP_LEVEL)
        # Consider the CTest module, which creates targets and options! Only needed if you want to enable submissions to a CDash server.
        include(CTest)
    endif()
    enable_testing()
    add_subdirectory(test)
endif()
if(ENABLE_BENCHMARK)
    add_subdirectory(benchmark)
endif()


option(FIXED_CONTAINERS_OPT_INSTALL "Enable install target" ${PROJECT_IS_TOP_LEVEL})
if (FIXED_CONTAINERS_OPT_INSTALL)
    target_include_directories(fixed_containers INTERFACE $<INSTALL_INTERFACE:include>)

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
            VERSION "0.0.0"
            COMPATIBILITY AnyNewerVersion
            ARCH_INDEPENDENT)

    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}Config)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
            DESTINATION lib/cmake/${PROJECT_NAME})

    install(EXPORT ${PROJECT_NAME}Config
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION lib/cmake/${PROJECT_NAME})

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
            DESTINATION .)

    export(EXPORT ${PROJECT_NAME}Config
            NAMESPACE ${PROJECT_NAME}::)
endif()

# If MSVC is being used, and ASAN is enabled, we need to set the debugger environment
# so that it behaves well with MSVC's debugger, and we can run the target from visual studio
if(MSVC)
    get_all_installable_targets(all_targets)
    message("all_targets=${all_targets}")
    set_target_properties(${all_targets} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
endif()