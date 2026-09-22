cmake_minimum_required(VERSION 3.29)
project(lb_oap22 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Qt из Homebrew (macOS). На других системах укажите свой путь через -DCMAKE_PREFIX_PATH
if(APPLE AND EXISTS /opt/homebrew/opt/qt)
    list(APPEND CMAKE_PREFIX_PATH /opt/homebrew/opt/qt)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(lb_oap22 WIN32 MACOSX_BUNDLE
    main.cpp
    PasswordChecker.h
    PasswordChecker.cpp
    MainWindow.h
    MainWindow.cpp
    StrengthMeter.h
    StrengthMeter.cpp
)

target_link_libraries(lb_oap22 PRIVATE Qt6::Widgets)

set_target_properties(lb_oap22 PROPERTIES
    OUTPUT_NAME PasswordChecker
    MACOSX_BUNDLE_BUNDLE_NAME "Password Checker"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.roskone.passwordchecker"
)
