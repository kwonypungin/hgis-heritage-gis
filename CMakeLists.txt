cmake_minimum_required(VERSION 3.16)

project(HGIS 
    VERSION 1.0.0
    DESCRIPTION "HGIS - Heritage GIS for Korean Cultural Heritage"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

# Create HGIS executable
add_executable(hgis src/app/main.cpp)

# Link Qt libraries
target_link_libraries(hgis
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

# Set target properties
set_target_properties(hgis PROPERTIES
    OUTPUT_NAME "hgis"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Install rules
install(TARGETS hgis
    RUNTIME DESTINATION bin
)

# Print configuration summary
message(STATUS "")
message(STATUS "HGIS Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Qt5 version: ${Qt5_VERSION}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")