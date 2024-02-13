set(SUBPROJECT_NAME "SBConverterApp")

add_subdirectory(core)
add_subdirectory(gui)

add_executable(${SUBPROJECT_NAME}
    songbook.cpp)

target_link_libraries(${SUBPROJECT_NAME} PUBLIC
    SBConverterCore
    SBConverterGui)

set_target_properties(${SUBPROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

