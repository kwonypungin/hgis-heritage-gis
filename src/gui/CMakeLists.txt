set(GUI_SOURCES
    HGISMainWindow.cpp
    HGISCrsSelectionDialog.cpp
    HGISMapCanvas.cpp
)

set(GUI_HEADERS
    HGISMainWindow.h
    HGISCrsSelectionDialog.h
    HGISMapCanvas.h
)

add_library(hgis_gui SHARED
    ${GUI_SOURCES}
    ${GUI_HEADERS}
)

target_link_libraries(hgis_gui
    hgis_core
    hgis_providers
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

target_compile_definitions(hgis_gui PRIVATE HGIS_GUI_EXPORT)