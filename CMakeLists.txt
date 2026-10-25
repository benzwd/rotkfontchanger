
cmake_minimum_required(VERSION 3.21)

project(ROTKFontChanger VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Test)

add_executable(ROTKFontChanger
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.hpp
    src/FontManager.cpp
    src/FontManager.hpp
    resources/fonts.qrc
)

target_link_libraries(ROTKFontChanger
    PRIVATE Qt6::Widgets
)

target_compile_definitions(ROTKFontChanger PRIVATE
    ROTK_APP_VERSION="${PROJECT_VERSION}"
)


include(CTest)

if(BUILD_TESTING)
    add_executable(FontManagerTests
        tests/FontManagerTests.cpp
        src/FontManager.cpp
        src/FontManager.hpp
        resources/fonts.qrc
    )

    target_include_directories(FontManagerTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_link_libraries(FontManagerTests PRIVATE
        Qt6::Core
        Qt6::Test
    )

    add_test(
        NAME FontManagerTests
        COMMAND FontManagerTests
    )
endif()
