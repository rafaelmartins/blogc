# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

if(CPACK_SOURCE_INSTALLED_DIRECTORIES)
    file(WRITE
        "${CMAKE_INSTALL_PREFIX}/version.cmake"
        "set(BLOGC_VERSION \"${CPACK_PACKAGE_VERSION}\")"
    )
endif()
