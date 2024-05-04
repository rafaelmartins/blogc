# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

set(executable_tests
    httpd_utils
    mime
)

if(BUILD_TESTING AND CMOCKA_FOUND)
    foreach(test ${executable_tests})
        add_cmocka_test(blogc-runserver_check_${test}
            SOURCES check_${test}.c
            LINK_LIBRARIES
                libblogc_runserver
                PkgConfig::CMOCKA
        )
    endforeach()

    set_property(TARGET blogc-runserver_check_httpd_utils
        PROPERTY LINK_FLAGS
        "-Wl,--wrap=read"
    )

    set_property(TARGET blogc-runserver_check_mime
        PROPERTY LINK_FLAGS
        "-Wl,--wrap=access"
    )
endif()
