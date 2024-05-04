# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED HAVE_DIRENT_H OR
   NOT DEFINED HAVE_ERRNO_H OR
   NOT DEFINED HAVE_LIBGEN_H OR
   NOT DEFINED HAVE_SYS_TYPES_H OR
   NOT DEFINED HAVE_SYS_STAT_H OR
   NOT DEFINED HAVE_TIME_H OR
   NOT DEFINED HAVE_UNISTD_H)
    message(FATAL_ERROR "Missing header dependencies for blogc-git-receiver")
endif()

add_library(libblogc_git_receiver STATIC
    post-receive.c
    post-receive.h
    pre-receive-parser.c
    pre-receive-parser.h
    pre-receive.c
    pre-receive.h
    settings.c
    settings.h
    shell-command-parser.c
    shell-command-parser.h
    shell.c
    shell.h
)

target_link_libraries(libblogc_git_receiver PRIVATE
    libblogc_common
)

add_executable(blogc-git-receiver
    main.c
)

target_link_libraries(blogc-git-receiver PRIVATE
    libblogc_git_receiver
)

install(TARGETS blogc-git-receiver)
