# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

add_library(libblogc STATIC
    content-parser.c
    content-parser.h
    datetime-parser.c
    datetime-parser.h
    debug.c
    debug.h
    filelist-parser.c
    filelist-parser.h
    funcvars.c
    funcvars.h
    loader.c
    loader.h
    renderer.c
    renderer.h
    rusage.c
    rusage.h
    source-parser.c
    source-parser.h
    sysinfo.c
    sysinfo.h
    template-parser.c
    template-parser.h
    toctree.c
    toctree.h
)

target_link_libraries(libblogc PRIVATE
    libblogc_common
    m
)

add_executable(blogc
    main.c
)

target_link_libraries(blogc PRIVATE
    libblogc
)

if(BUILD_BLOGC_MAKE_EMBEDDED)
    target_sources(blogc PRIVATE
        ../blogc-make/main.c
    )

    target_link_libraries(blogc PRIVATE
        libblogc_make
    )

    target_compile_definitions(blogc PRIVATE
        MAKE_EMBEDDED
    )
endif()

install(TARGETS blogc)
