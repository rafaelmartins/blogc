# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

if(NOT CPACK_SOURCE_INSTALLED_DIRECTORIES)
    if(CPACK_SYSTEM_NAME MATCHES "^win")
        set(_license "LICENSE.txt")
    else()
        set(_license "LICENSE")
    endif()

    file(COPY_FILE
        "${CPACK_RESOURCE_FILE_LICENSE}"
        "${CMAKE_INSTALL_PREFIX}/${_license}"
    )

    file(COPY_FILE
        "${CPACK_RESOURCE_FILE_README}"
        "${CMAKE_INSTALL_PREFIX}/README.md"
    )
endif()
