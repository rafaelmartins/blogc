# SPDX-FileCopyrightText: 2022 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED HAVE_ARPA_INET_H OR
   NOT DEFINED HAVE_FCNTL_H OR
   NOT DEFINED HAVE_LIMITS_H OR
   NOT DEFINED HAVE_NETINET_IN_H OR
   NOT DEFINED HAVE_SIGNAL_H OR
   NOT DEFINED HAVE_SYS_SOCKET_H OR
   NOT DEFINED HAVE_SYS_STAT_H OR
   NOT DEFINED HAVE_SYS_TYPES_H OR
   NOT DEFINED HAVE_UNISTD_H)
    message(FATAL_ERROR "Missing header dependencies for blogc-runserver")
endif()

add_library(libblogc_runserver STATIC
    httpd-utils.c
    httpd-utils.h
    httpd.c
    httpd.h
    mime.c
    mime.h
)

target_link_libraries(libblogc_runserver PRIVATE
    libblogc_common
)

add_executable(blogc-runserver
    main.c
)

target_link_libraries(blogc-runserver PRIVATE
    libblogc_runserver
)

install(TARGETS blogc-runserver)
