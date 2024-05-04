# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED HAVE_DIRENT_H OR
   NOT DEFINED HAVE_FCNTL_H OR
   NOT DEFINED HAVE_LIBGEN_H OR
   NOT DEFINED HAVE_SYS_STAT_H OR
   NOT DEFINED HAVE_SYS_WAIT_H OR
   NOT DEFINED HAVE_TIME_H OR
   NOT DEFINED HAVE_UNISTD_H)
    message(FATAL_ERROR "Missing header dependencies for blogc-make")
endif()

add_library(libblogc_make STATIC
    atom.c
    atom.h
    ctx.c
    ctx.h
    exec.c
    exec.h
    exec-native.c
    exec-native.h
    httpd.c
    httpd.h
    reloader.c
    reloader.h
    rules.c
    rules.h
    settings.c
    settings.h
    utils.c
    utils.h
)

target_link_libraries(libblogc_make PRIVATE
    libblogc_common
    m
)

if(NOT BUILD_BLOGC_MAKE_EMBEDDED)
    add_executable(blogc-make
        main.c
    )

    target_link_libraries(blogc-make PRIVATE
        libblogc_make
    )

    install(TARGETS blogc-make)
endif()
