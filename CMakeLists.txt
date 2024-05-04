# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.18)

project(blogc C)

# explicitly disable tests by default.
option(BUILD_TESTING "Build the testing tree." OFF)

include(CheckFunctionExists)
include(CheckIncludeFile)
include(GNUInstallDirs)
include(cmake/AddCMockaTest.cmake)

check_function_exists(getrusage HAVE_GETRUSAGE)
check_function_exists(gethostname HAVE_GETHOSTNAME)

check_include_file(netdb.h HAVE_NETDB_H)
check_include_file(sysexits.h HAVE_SYSEXITS_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_include_file(sys/resource.h HAVE_SYS_RESOURCE_H)
check_include_file(sys/wait.h HAVE_SYS_WAIT_H)
check_include_file(time.h HAVE_TIME_H)
check_include_file(unistd.h HAVE_UNISTD_H)

configure_file(config.h.in config.h @ONLY)

find_program(BASH bash HINTS /bin /usr/bin)

include_directories(${CMAKE_BINARY_DIR})
add_compile_definitions(HAVE_CONFIG_H)

add_subdirectory(man)
add_subdirectory(src)
add_subdirectory(tests)
