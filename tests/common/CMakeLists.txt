# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

blogc_executable_test(blogc_common config_parser)
blogc_executable_test(blogc_common error)
blogc_executable_test(blogc_common sort)
blogc_executable_test(blogc_common stdin
    WRAP
        fgetc
)
blogc_executable_test(blogc_common utf8)
blogc_executable_test(blogc_common utils)
