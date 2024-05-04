# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

blogc_executable_test(blogc_runserver httpd_utils
    WRAP
        read
)
blogc_executable_test(blogc_runserver mime
    WRAP
        access
)
