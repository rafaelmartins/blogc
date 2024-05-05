# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

blogc_executable_test(blogc_make atom)
blogc_executable_test(blogc_make exec
    WRAP
        access
)
blogc_executable_test(blogc_make rules)
blogc_executable_test(blogc_make settings)
blogc_executable_test(blogc_make utils)
blogc_script_test(blogc_make blogc_make)
