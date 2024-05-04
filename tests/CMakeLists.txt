# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

find_package(PkgConfig REQUIRED)
pkg_check_modules(CMOCKA IMPORTED_TARGET cmocka)

add_subdirectory(common)
add_subdirectory(blogc)

if(BUILD_BLOGC_MAKE OR BUILD_BLOGC_MAKE_EMBEDDED)
    add_subdirectory(blogc-make)
endif()

if(BUILD_BLOGC_RUNSERVER)
    add_subdirectory(blogc-runserver)
endif()

if(BUILD_BLOGC_GIT_RECEIVER)
    add_subdirectory(blogc-git-receiver)
endif()
