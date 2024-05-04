# SPDX-FileCopyrightText: 2022 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

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
