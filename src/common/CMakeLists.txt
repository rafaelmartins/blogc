# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

add_library(libblogc_common STATIC
    compat.c
    compat.h
    config-parser.c
    config-parser.h
    error.c
    error.h
    file.c
    file.h
    sort.c
    sort.h
    stdin.c
    stdin.h
    utf8.c
    utf8.h
    utils.c
    utils.h
)
