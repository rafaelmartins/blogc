# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

if(NOT Git_FOUND)
    message(FATAL_ERROR "blogc-git-receiver tests require Git.")
endif()

find_program(MAKE make gmake REQUIRED)
find_program(TAR tar REQUIRED)

blogc_executable_test(blogc_git_receiver pre_receive_parser)
blogc_executable_test(blogc_git_receiver settings
    WRAP
        realpath
)
blogc_executable_test(blogc_git_receiver shell_command_parser)
blogc_script_test(blogc_git_receiver post_receive)
blogc_script_test(blogc_git_receiver pre_receive)
blogc_script_test(blogc_git_receiver shell)
