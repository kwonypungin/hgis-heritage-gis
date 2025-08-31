cmake_minimum_required(VERSION 3.16)

project(HGIS 
    VERSION 1.0.0
    DESCRIPTION "HGIS - Heritage GIS based on QGIS"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Debug configuration
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")

# Find required packages
find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    Xml
    Sql
    Svg
    PrintSupport
    Concurrent
)

# Find QGIS components
find_path(QGIS_INCLUDE_DIR 
    NAMES qgis.h qgsapplication.h
    PATHS
        /usr/include/qgis
        /usr/local/include/qgis
        /opt/qgis/include
    DOC "QGIS include directory"
)

find_library(QGIS_CORE_LIBRARY
    NAMES qgis_core libqgis_core
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/qgis/lib
    DOC "QGIS core library"
)

find_library(QGIS_GUI_LIBRARY
    NAMES qgis_gui libqgis_gui
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/qgis/lib
    DOC "QGIS GUI library"
)

find_library(QGIS_APP_LIBRARY
    NAMES qgis_app libqgis_app
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/qgis/lib
    DOC "QGIS app library"
)

# Find GDAL
find_package(GDAL REQUIRED)

# Find PROJ
find_package(PROJ REQUIRED)

# Find GEOS
find_library(GEOS_LIBRARY
    NAMES geos_c libgeos_c
    PATHS
        /usr/lib
        /usr/local/lib
    DOC "GEOS library"
)

# Check if all required libraries are found
if(NOT QGIS_INCLUDE_DIR)
    message(FATAL_ERROR "QGIS include directory not found! Install QGIS development packages.")
endif()

if(NOT QGIS_CORE_LIBRARY)
    message(FATAL_ERROR "QGIS core library not found! Install QGIS development packages.")
endif()

if(NOT QGIS_GUI_LIBRARY)
    message(FATAL_ERROR "QGIS GUI library not found! Install QGIS development packages.")
endif()

# Print found libraries for debugging
message(STATUS "QGIS include directory: ${QGIS_INCLUDE_DIR}")
message(STATUS "QGIS core library: ${QGIS_CORE_LIBRARY}")
message(STATUS "QGIS GUI library: ${QGIS_GUI_LIBRARY}")
if(QGIS_APP_LIBRARY)
    message(STATUS "QGIS app library: ${QGIS_APP_LIBRARY}")
endif()
message(STATUS "GDAL libraries: ${GDAL_LIBRARIES}")
message(STATUS "PROJ libraries: ${PROJ_LIBRARIES}")

# Include directories
include_directories(
    ${QGIS_INCLUDE_DIR}
    ${QGIS_INCLUDE_DIR}/qgis
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core  
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
)

# Source files
set(HGIS_SOURCES
    src/app/main.cpp
    src/app/hgisapp.cpp
)

set(HGIS_HEADERS
    src/app/hgisapp.h
)

# Create HGIS executable
add_executable(hgis 
    ${HGIS_SOURCES} 
    ${HGIS_HEADERS}
)

# Link libraries
target_link_libraries(hgis
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    Qt5::Xml
    Qt5::Sql
    Qt5::Svg
    Qt5::PrintSupport
    Qt5::Concurrent
    ${QGIS_CORE_LIBRARY}
    ${QGIS_GUI_LIBRARY}
    ${GDAL_LIBRARIES}
)

# Link QGIS app library if found
if(QGIS_APP_LIBRARY)
    target_link_libraries(hgis ${QGIS_APP_LIBRARY})
endif()

# Link PROJ library
if(PROJ_LIBRARIES)
    target_link_libraries(hgis ${PROJ_LIBRARIES})
elseif(TARGET PROJ::proj)
    target_link_libraries(hgis PROJ::proj)
endif()

# Link GEOS library if found
if(GEOS_LIBRARY)
    target_link_libraries(hgis ${GEOS_LIBRARY})
endif()

# Include GDAL headers
if(GDAL_INCLUDE_DIRS)
    target_include_directories(hgis PRIVATE ${GDAL_INCLUDE_DIRS})
endif()

# Set target properties
set_target_properties(hgis PROPERTIES
    OUTPUT_NAME "hgis"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Install rules
install(TARGETS hgis
    RUNTIME DESTINATION bin
)

# Install Python modules (optional)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/python/hgis")
    install(DIRECTORY python/hgis
        DESTINATION share/hgis/python
        FILES_MATCHING PATTERN "*.py"
    )
endif()

# Install resources (optional)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
    install(DIRECTORY resources
        DESTINATION share/hgis
    )
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "HGIS Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  QGIS support: YES")
message(STATUS "  GDAL support: YES")
message(STATUS "  PROJ support: ${PROJ_LIBRARIES}")
message(STATUS "  GEOS support: ${GEOS_LIBRARY}")
message(STATUS "")

