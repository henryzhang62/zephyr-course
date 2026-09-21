cmake_minimum_required(VERSION 3.20.0)

list(APPEND DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
find_package(Zephyr REQUIRED)
project(my_zephyr_app)

add_subdirectory(drivers/sensor/led_sensor)
target_include_directories(app PRIVATE drivers/sensor/led_sensor)
target_sources(app PRIVATE src/main.c)
