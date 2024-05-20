# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.19)

find_package(Git)

if (EXISTS "${CMAKE_SOURCE_DIR}/version.cmake")
    include("${CMAKE_SOURCE_DIR}/version.cmake")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/.git" AND Git_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --abbrev=4 HEAD
        OUTPUT_VARIABLE _blogc_version
        ERROR_VARIABLE _blogc_version_err
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE _blogc_version_result
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(_blogc_version_err)
        message(FATAL_ERROR "Failed to find version from Git\n${_blogc_version_err}")
    endif()

    if(NOT _blogc_version_result EQUAL 0)
        message(FATAL_ERROR "Failed to find version from Git. Git process returned ${_blogc_version_result}")
    endif()

    string(REGEX REPLACE "^v" "" _blogc_version "${_blogc_version}")
    string(REGEX REPLACE "^([^-]*)-(.*)" "\\1.\\2" _blogc_version "${_blogc_version}")
    string(REGEX REPLACE "-g" "-" BLOGC_VERSION "${_blogc_version}")
else()
    message(FATAL_ERROR "Can't find version information!")
endif()

string(REGEX MATCHALL "[0-9]+" _blogc_version_list "${BLOGC_VERSION}")
list(LENGTH _blogc_version_list _blogc_version_list_len)
if(NOT _blogc_version_list_len GREATER_EQUAL 3)
    message(FATAL_ERROR "Invalid version: ${BLOGC_VERSION}")
endif()

list(GET _blogc_version_list 0 _blogc_version_major)
list(GET _blogc_version_list 1 _blogc_version_minor)
list(GET _blogc_version_list 2 _blogc_version_patch)
set(BLOGC_VERSION_CANONICAL "${_blogc_version_major}.${_blogc_version_minor}.${_blogc_version_patch}")

if(EXISTS "${CMAKE_SOURCE_DIR}/.git" AND _blogc_version_list_len GREATER_EQUAL 4)
    list(GET _blogc_version_list 3 _blogc_version_tweak)
    set(BLOGC_VERSION_CANONICAL "${BLOGC_VERSION_CANONICAL}.${_blogc_version_tweak}")
endif()

project(blogc
    VERSION "${BLOGC_VERSION_CANONICAL}"
    HOMEPAGE_URL "https://blogc.rgm.io/"
    LANGUAGES C
)

# explicitly disable tests by default.
option(BUILD_TESTING "Build the testing tree." OFF)

enable_testing()

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CTest)
include(GNUInstallDirs)

check_function_exists(getrusage HAVE_GETRUSAGE)
check_function_exists(gethostname HAVE_GETHOSTNAME)

check_include_file(arpa/inet.h HAVE_ARPA_INET_H)
check_include_file(dirent.h HAVE_DIRENT_H)
check_include_file(errno.h HAVE_ERRNO_H)
check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(libgen.h HAVE_LIBGEN_H)
check_include_file(limits.h HAVE_LIMITS_H)
check_include_file(netdb.h HAVE_NETDB_H)
check_include_file(netinet/in.h HAVE_NETINET_IN_H)
check_include_file(signal.h HAVE_SIGNAL_H)
check_include_file(sysexits.h HAVE_SYSEXITS_H)
check_include_file(sys/resource.h HAVE_SYS_RESOURCE_H)
check_include_file(sys/socket.h HAVE_SYS_SOCKET_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(sys/wait.h HAVE_SYS_WAIT_H)
check_include_file(time.h HAVE_TIME_H)
check_include_file(unistd.h HAVE_UNISTD_H)

set(PACKAGE_VERSION "${BLOGC_VERSION}")
configure_file(config.h.in config.h @ONLY)

include_directories(${CMAKE_BINARY_DIR})
add_compile_definitions(HAVE_CONFIG_H)

add_subdirectory(man)
add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_PACKAGE_VENDOR "Rafael G. Martins")
set(CPACK_PACKAGE_VERSION "${BLOGC_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A blog compiler.")
set(CPACK_PACKAGE_CHECKSUM SHA512)

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_INSTALL_SCRIPTS
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack/install_license_and_readme.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack/install_version.cmake"
)

set(CPACK_SOURCE_GENERATOR "TXZ")
set(CPACK_SOURCE_IGNORE_FILES
    \\.git
    \\.gitignore
    \\.github/
    build/
    build-win/
    Testing/
    ".*~$"
)
if(DEFINED CPACK_PACKAGE_NAME)
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${BLOGC_VERSION}")
else()
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${BLOGC_VERSION}")
endif()

if(DEFINED CPACK_PACKAGE_FILE_NAME_SUFFIX)
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_SOURCE_PACKAGE_FILE_NAME}-${CPACK_PACKAGE_FILE_NAME_SUFFIX}")
endif()

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
endif()

if(UNIX)
    set(CPACK_GENERATOR "STGZ" "TXZ")
endif()

include(CPack)
