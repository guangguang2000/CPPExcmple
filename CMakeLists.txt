cmake_minimum_required(VERSION 3.14)

project(chart5 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Charts Core Gui)

add_executable(chart5
  main.cpp
  candlestickdatareader.cpp candlestickdatareader.h
)

target_link_libraries(chart5
    Qt::Charts
    Qt::Core
    Qt::Gui
)

set_source_files_properties("acme_data.txt"
    PROPERTIES QT_RESOURCE_ALIAS "acme"
)

qt6_add_resources(chart5 "chart5data"
    PREFIX
        "/"
    FILES
       "acme_data.txt"
)
