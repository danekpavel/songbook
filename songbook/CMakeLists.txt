set(SUBPROJECT_NAME "songbook")

add_subdirectory(core)
add_subdirectory(gui)

# the .qrc file will be automatically processed with rcc
set(CMAKE_AUTORCC ON)

add_executable(${SUBPROJECT_NAME}
    songbook.cpp
    resources/resources.qrc)

target_link_libraries(${SUBPROJECT_NAME} PUBLIC
    SBConverterCore
    SBConverterGui)

set_target_properties(${SUBPROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

