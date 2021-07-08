cmake_minimum_required(VERSION 3.5)

project(test LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# complier
if (WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()
# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()
set(CMAKE_PREFIX_PATH /opt/Qt5.12.9/5.12.9/gcc_64)
find_package(Qt5 COMPONENTS Widgets Network Core Gui REQUIRED)

#HttpClient
include_directories("${PROJECT_SOURCE_DIR}/HttpClient")
add_subdirectory(HttpClient)
#ImageViewLabel
include_directories("${PROJECT_SOURCE_DIR}/ImageViewLabel")
add_subdirectory(ImageViewLabel)
#PdfGenerator
include_directories("${PROJECT_SOURCE_DIR}/PdfGenerator")
add_subdirectory(PdfGenerator)
#SingleApplication
include_directories("${PROJECT_SOURCE_DIR}/SingleApplication")
add_subdirectory(SingleApplication)
#CustomButton
include_directories("${PROJECT_SOURCE_DIR}/CustomButton")
add_subdirectory(CustomButton)
#IpScanner
include_directories("${PROJECT_SOURCE_DIR}/IpScanner")
add_subdirectory(IpScanner)
#ProgressButton
include_directories("${PROJECT_SOURCE_DIR}/ProgressButton")
add_subdirectory(ProgressButton)

if(ANDROID)
  add_library(test SHARED
    ./test/main.cpp
    ./test/mainwindow.cpp
    ./test/mainwindow.h
    ./test/mainwindow.ui
  )
else()
  add_executable(test
    ./test/main.cpp
    ./test/mainwindow.cpp
    ./test/mainwindow.h
    ./test/mainwindow.ui
  )
endif()

target_link_libraries(test PRIVATE Qt5::Widgets Qt5::Network Qt5::Core Qt5::Gui)
