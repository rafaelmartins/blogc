# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

set(executable_tests
    content_parser
    datetime_parser
    filelist_parser
    funcvars
    loader
    renderer
    rusage
    source_parser
    sysinfo
    sysinfo2
    template_parser
    toctree
)

set(script_tests
    blogc
)

if(BUILD_TESTING AND CMOCKA_FOUND)
    foreach(test ${executable_tests})
        add_cmocka_test(blogc_check_${test}
            SOURCES check_${test}.c
            LINK_LIBRARIES
                libblogc
                PkgConfig::CMOCKA
        )
    endforeach()

    set_property(TARGET blogc_check_funcvars
        PROPERTY LINK_FLAGS
        "-Wl,--wrap=bc_file_get_contents"
    )

    set_property(TARGET blogc_check_loader
        PROPERTY LINK_FLAGS
        "-Wl,--wrap=bc_file_get_contents"
    )

    if(HAVE_SYS_RESOURCE_H AND HAVE_SYS_TIME_H)
        set_property(TARGET blogc_check_rusage
            PROPERTY LINK_FLAGS
            "-Wl,--wrap=getrusage"
        )
    endif()

    set_property(TARGET blogc_check_sysinfo
        PROPERTY LINK_FLAGS
        "-Wl,--wrap=bc_file_get_contents -Wl,--wrap=getenv"
    )
    if(HAVE_NETDB_H)
        set_property(TARGET blogc_check_sysinfo
            APPEND_STRING
            PROPERTY LINK_FLAGS
            " -Wl,--wrap=gethostbyname"
        )
    endif()
    if(HAVE_UNISTD_H)
        set_property(TARGET blogc_check_sysinfo
            APPEND_STRING
            PROPERTY LINK_FLAGS
            " -Wl,--wrap=gethostname"
        )
    endif()
    if(HAVE_TIME_H)
        set_property(TARGET blogc_check_sysinfo
            APPEND_STRING
            PROPERTY LINK_FLAGS
            " -Wl,--wrap=time -Wl,--wrap=gmtime"
        )
    endif()
endif()

if(BUILD_TESTING)
    foreach(test ${script_tests})
        configure_file(check_${test}.sh.in blogc_check_${test}.sh @ONLY)
        add_test(NAME blogc_check_${test}.sh
            COMMAND blogc_check_${test}.sh
        )
    endforeach()
endif()
