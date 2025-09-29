cmake_minimum_required(VERSION 3.10)
project(radar_ppi VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

# Add subprojects
add_subdirectory(core)
add_subdirectory(opengl)
add_subdirectory(test_app)

# Installation setup
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Export + install target set
install(
    EXPORT radarTargets
    NAMESPACE radar::
    FILE radarTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/radar
)

# Generate Config and ConfigVersion
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/radarConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/radarConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/radar
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/radarConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install the config files
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/radarConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/radarConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/radar
)
