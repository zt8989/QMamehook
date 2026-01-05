cmake_minimum_required(VERSION 3.14)

project(QMamehook VERSION 2.2 LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(QMH_GITHASH "Sets QMamehook git hash" OFF)

if(QMH_GITHASH)
    add_compile_definitions(QMH_GITHASH="${QMH_GITHASH}")
endif()

add_compile_definitions(QMH_VERSION="${PROJECT_VERSION}")

if(NOT QMH_QT_VERSION)
    find_package(QT NAMES Qt6 Qt5 REQUIRED)
else()
    find_package(QT NAMES ${QMH_QT_VERSION} REQUIRED)
endif()

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network SerialPort)

set(PROJECT_SOURCES
    main.cpp
    qhookermain.h
    qhookermain.cpp
    qhookerquickjs.h
    qhookerquickjs.cpp
    xinput_wrapper.h
    xinput_wrapper.cpp
)

set(QUICKJS_SOURCES
    quickjs/quickjs.c
    quickjs/libregexp.c
    quickjs/libunicode.c
    quickjs/cutils.c
    quickjs/dtoa.c
)

set_source_files_properties(${QUICKJS_SOURCES} PROPERTIES LANGUAGE C)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QMamehook
      MANUAL_FINALIZATION
      ${PROJECT_SOURCES}
      ${QUICKJS_SOURCES}
    )
else()
    add_executable(QMamehook
      ${PROJECT_SOURCES}
      ${QUICKJS_SOURCES}
    )
endif()

target_link_libraries(QMamehook PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::SerialPort)
target_include_directories(QMamehook PRIVATE quickjs)

include(GNUInstallDirs)
install(TARGETS QMamehook
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_finalize_executable(QMamehook)
endif()
