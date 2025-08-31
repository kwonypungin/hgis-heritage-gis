set(PROVIDERS_SOURCES
    HGISGdalProvider.cpp
)

set(PROVIDERS_HEADERS
    HGISGdalProvider.h
)

add_library(hgis_providers SHARED
    ${PROVIDERS_SOURCES}
    ${PROVIDERS_HEADERS}
)

target_link_libraries(hgis_providers
    Qt5::Core
    ${GDAL_LIBRARIES}
)

target_include_directories(hgis_providers PUBLIC
    ${Qt5Core_INCLUDE_DIRS}
    ${GDAL_INCLUDE_DIRS}
)

if(GDAL_FOUND)
    target_compile_definitions(hgis_providers PRIVATE GDAL_VERSION_NUM=${GDAL_VERSION_NUM})
endif()

target_compile_definitions(hgis_providers PRIVATE HGIS_PROVIDERS_EXPORT)