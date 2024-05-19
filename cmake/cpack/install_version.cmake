if(CPACK_SOURCE_INSTALLED_DIRECTORIES)
    file(WRITE
        "${CMAKE_INSTALL_PREFIX}/version.cmake"
        "set(BLOGC_VERSION \"${CPACK_PACKAGE_VERSION}\")"
    )
endif()
