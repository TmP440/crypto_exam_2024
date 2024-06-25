cmake_minimum_required(VERSION 3.5)

project(crypto_exam_2024 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(CMAKE_BUILD_TYPE Debug)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(crypto_exam_2024
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        game_state.json
    )
else()
    if(ANDROID)
        add_library(crypto_exam_2024 SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(crypto_exam_2024
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(crypto_exam_2024 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(crypto_exam_2024 PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.crypto_exam_2024
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS crypto_exam_2024
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(crypto_exam_2024)
endif()
