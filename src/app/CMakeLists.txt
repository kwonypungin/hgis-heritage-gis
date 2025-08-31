set(APP_SOURCES
    main.cpp
)

add_executable(hgis
    ${APP_SOURCES}
)

target_link_libraries(hgis
    hgis_core
    hgis_gui
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

if(WIN32)
    set_target_properties(hgis PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

install(TARGETS hgis
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)