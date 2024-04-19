cmake_minimum_required(VERSION 3.16)
project(RandomUnsplash LANGUAGES CXX)

if(NOT DEFINED INSTALL_DIR)
    set(INSTALL_DIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_DIR}/RandomUnsplash")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Widgets)

qt_standard_project_setup()

qt_add_executable(http

    httpwindow.cpp httpwindow.h
    main.cpp
    utils.h
    utils.cpp
)

set_target_properties(http PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(http PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Widgets
)

install(TARGETS http
    RUNTIME DESTINATION "${INSTALL_DIR}"
    BUNDLE DESTINATION "${INSTALL_DIR}"
    LIBRARY DESTINATION "${INSTALL_DIR}"
)
