# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

blogc_executable_test(blogc_git_receiver pre_receive_parser)
blogc_executable_test(blogc_git_receiver settings
    WRAP
        realpath
)
blogc_executable_test(blogc_git_receiver shell_command_parser)
blogc_script_test(blogc_git_receiver post_receive)
blogc_script_test(blogc_git_receiver pre_receive)
blogc_script_test(blogc_git_receiver shell)
