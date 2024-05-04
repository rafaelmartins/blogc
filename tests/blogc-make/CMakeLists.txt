# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

set(executable_tests
    atom
    exec
    rules
    settings
    utils
)

set(script_tests
    blogc_make
)

if(BUILD_TESTING AND CMOCKA_FOUND)
    foreach(test ${executable_tests})
        add_cmocka_test(blogc-make_check_${test}
            SOURCES check_${test}.c
            LINK_LIBRARIES
                libblogc_make
                PkgConfig::CMOCKA
        )
    endforeach()

    set_property(TARGET blogc-make_check_exec
        PROPERTY LINK_FLAGS
        "-Wl,--wrap=access"
    )
endif()

if(BUILD_TESTING)
    foreach(test ${script_tests})
        configure_file(check_${test}.sh.in blogc-make_check_${test}.sh @ONLY)
        add_test(NAME blogc-make_check_${test}.sh
            COMMAND blogc-make_check_${test}.sh
        )
    endforeach()
endif()
