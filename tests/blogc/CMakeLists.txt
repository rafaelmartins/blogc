# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

blogc_executable_test(blogc content_parser)
blogc_executable_test(blogc datetime_parser)
blogc_executable_test(blogc filelist_parser)
blogc_executable_test(blogc funcvars
    WRAP
        bc_file_get_contents
)
blogc_executable_test(blogc loader
    WRAP
        bc_file_get_contents
)
blogc_executable_test(blogc renderer)
blogc_executable_test(blogc rusage
    WRAP
        getrusage
)
blogc_executable_test(blogc source_parser)
blogc_executable_test(blogc sysinfo
    WRAP
        bc_file_get_contents
        getenv
        gethostbyname
        gethostname
        gmtime
        time
)
blogc_executable_test(blogc sysinfo2)
blogc_executable_test(blogc template_parser)
blogc_executable_test(blogc toctree)
blogc_script_test(blogc blogc)
