cmake_minimum_required(VERSION 3.20)
project(songbook)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")

find_package(XercesC CONFIG REQUIRED)
include_directories(${XercesC_INCLUDE_DIRS})

add_subdirectory(songbook)

# Generate doxygen documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_INDEX ${CMAKE_CURRENT_SOURCE_DIR}/doc/html/index.html)
    file(GLOB header_files ${CMAKE_CURRENT_SOURCE_DIR}/songbook/*.hpp)

    add_custom_command(
        OUTPUT ${DOXYGEN_INDEX} 
        COMMAND doxygen
        DEPENDS ${header_files}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
        VERBATIM
    )

    add_custom_target(documentation
        ALL
        DEPENDS ${DOXYGEN_INDEX}
    )
else()
    message("Doxygen not found -- documentation will not be generated")
endif()


