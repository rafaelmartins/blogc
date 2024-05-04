# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

option(BUILD_MANPAGES "Build manpages." OFF)
if(BUILD_MANPAGES)
    find_program(RONN ronn REQUIRED)

    set(man1
        blogc.1
        blogc-git-receiver.1
        blogc-make.1
        blogc-runserver.1
    )

    set(man5
        blogcfile.5
    )

    set(man7
        blogc-pagination.7
        blogc-source.7
        blogc-template.7
        blogc-toctree.7
    )

    set(manpages
        ${man1}
        ${man5}
        ${man7}
    )

    foreach(man ${manpages})
        add_custom_command(
            OUTPUT ${man}
            COMMAND
                ${RONN}
                    --roff
                    --organization "Rafael G. Martins"
                    --manual "blogc Manual"
                    --output-dir ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/${man}.ronn
            DEPENDS ${man}.ronn
        )

        add_custom_target(man-${man}
            ALL
            DEPENDS ${man}
        )
    endforeach()

    foreach(man ${man1})
        install(
            FILES ${CMAKE_CURRENT_BINARY_DIR}/${man}
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
        )
    endforeach()
    foreach(man ${man5})
        install(
            FILES ${CMAKE_CURRENT_BINARY_DIR}/${man}
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man5
        )
    endforeach()
    foreach(man ${man7})
        install(
            FILES ${CMAKE_CURRENT_BINARY_DIR}/${man}
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man7
        )
    endforeach()
endif()
