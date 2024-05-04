# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

set(executable_tests
    config_parser
    error
    sort
    stdin
    utf8
    utils
)

if(BUILD_TESTING AND CMOCKA_FOUND)
    foreach(test ${executable_tests})
        add_cmocka_test(common_check_${test}
            SOURCES check_${test}.c
            LINK_LIBRARIES
                libblogc_common
                PkgConfig::CMOCKA
        )
    endforeach()

    set_property(TARGET common_check_stdin
        PROPERTY LINK_FLAGS
        "-Wl,--wrap=fgetc"
    )
endif()
