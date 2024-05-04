# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory(common)

option(BUILD_BLOGC_MAKE "Build blogc-make binary." OFF)
option(BUILD_BLOGC_MAKE_EMBEDDED "Build blogc-make embedded into blogc binary." OFF)
if(BUILD_BLOGC_MAKE OR BUILD_BLOGC_MAKE_EMBEDDED)
    add_subdirectory(blogc-make)
endif()

option(BUILD_BLOGC_RUNSERVER "Build blogc-runserver binary." OFF)
if(BUILD_BLOGC_RUNSERVER)
    add_subdirectory(blogc-runserver)
endif()

option(BUILD_BLOGC_GIT_RECEIVER "Build blogc-git-receiver binary." OFF)
if(BUILD_BLOGC_GIT_RECEIVER)
    add_subdirectory(blogc-git-receiver)
endif()

add_subdirectory(blogc)
