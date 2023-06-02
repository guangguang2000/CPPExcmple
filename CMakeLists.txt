cmake_minimum_required(VERSION 3.14)

project(strategy_chart LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Charts Gui)

add_executable(ctaform
  main.cpp
  form.h
  form.cpp  
  marketchart.h
  marketchart.cpp
  chartview.h
  chartview.cpp
)
target_link_libraries(ctaform Qt::Core Qt::Sql Qt::Charts Qt::Gui)
