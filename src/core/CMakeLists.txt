set(CORE_SOURCES
    HGISApplication.cpp
    HGISMapCanvas.cpp
    HGISCoordinateReferenceSystem.cpp
    HGISCoordinateTransform.cpp
    HGISMapLayer.cpp
    HGISVectorLayer.cpp
    HGISLayerManager.cpp
)

set(CORE_HEADERS
    HGISApplication.h
    HGISMapCanvas.h
    HGISCoordinateReferenceSystem.h
    HGISCoordinateTransform.h
    HGISMapLayer.h
    HGISVectorLayer.h
    HGISLayerManager.h
)

add_library(hgis_core SHARED
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

target_link_libraries(hgis_core
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    hgis_providers
)

target_include_directories(hgis_core PUBLIC
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

if(GDAL_FOUND)
    target_link_libraries(hgis_core ${GDAL_LIBRARIES})
    target_include_directories(hgis_core PRIVATE ${GDAL_INCLUDE_DIRS})
    target_compile_definitions(hgis_core PRIVATE GDAL_VERSION_NUM=${GDAL_VERSION_NUM})
endif()

if(PROJ_FOUND)
    target_link_libraries(hgis_core ${PROJ_LIBRARIES})
    target_include_directories(hgis_core PRIVATE ${PROJ_INCLUDE_DIRS})
    target_compile_definitions(hgis_core PRIVATE PROJ_VERSION_MAJOR=${PROJ_VERSION_MAJOR})
endif()

target_compile_definitions(hgis_core PRIVATE HGIS_CORE_EXPORT)