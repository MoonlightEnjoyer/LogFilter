cmake_minimum_required(VERSION 3.8)

include(CMakePrintHelpers)

project(LogFilter VERSION 0.1 LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)

message("Before variables set")

#set(CMAKE_CXX_FLAGS "-I\ /home/dude/Desktop/LogFilter/LogFilter/re2_win/abseil-cpp/")
#set(absl_DIR "/home/dude/Desktop/LogFilter/LogFilter/re2_win/abseil-cpp/")

find_package(absl)
find_package(re2)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

message("After find packages")

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)



if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(LogFilter
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        filecontext.h
        filecontext.cpp
        simpleregex.cpp
        simpleregex.h
        fileProcessWorker.h
        fileProcessWorker.cpp
    )


# Define target properties for Android with Qt 6 as:
#    set_property(TARGET LogFilter APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(LogFilter SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(LogFilter
            ${PROJECT_SOURCES}
        )
    endif()
endif()

include_directories(/home/dude/Desktop/hyperscan/src/)

target_link_libraries(LogFilter PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_link_libraries(LogFilter PRIVATE re2)

target_link_libraries(LogFilter PRIVATE "/home/dude/Desktop/hyperscan/build/lib/libhs.a")


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.LogFilter)
endif()
set_target_properties(LogFilter PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS LogFilter
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(LogFilter)
endif()
