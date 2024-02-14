set(SUBPROJECT_NAME "SBConverterGui")

qt_standard_project_setup()

add_library(${SUBPROJECT_NAME} STATIC
    MainWindow.cpp)

target_include_directories(${SUBPROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${SUBPROJECT_NAME}
    SBConverterCore
    Qt6::Widgets)


