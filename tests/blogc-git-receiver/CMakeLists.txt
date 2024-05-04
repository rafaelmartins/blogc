# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

set(executable_tests
    pre_receive_parser
    settings
    shell_command_parser
)

set(script_tests
    post_receive
    pre_receive
    shell
)

if(BUILD_TESTING AND CMOCKA_FOUND)
    foreach(test ${executable_tests})
        add_cmocka_test(blogc-git-receiver_check_${test}
            SOURCES check_${test}.c
            LINK_LIBRARIES
                libblogc_git_receiver
                PkgConfig::CMOCKA
        )
    endforeach()

    set_property(TARGET blogc-git-receiver_check_settings
        PROPERTY LINK_FLAGS
        "-Wl,--wrap=realpath"
    )
endif()

if(BUILD_TESTING)
    foreach(test ${script_tests})
        configure_file(check_${test}.sh.in blogc-git-receiver_check_${test}.sh @ONLY)
        add_test(NAME blogc-git-receiver_check_${test}.sh
            COMMAND blogc-git-receiver_check_${test}.sh
        )
    endforeach()
endif()
