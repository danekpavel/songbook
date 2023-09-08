project(songbook)

# CPP file with XML schema definition
set(XML_SCHEMA_CPP xmlSchema.cpp)

add_executable(songbook
	songbook.cpp
	SongbookConverter.cpp
    SongbookParser.cpp
    SongbookException.cpp
    SongbookErrorHandler.cpp
	Song.cpp
	SongbookPrinter.cpp
	SongbookPrinterLatex.cpp
    ${XML_SCHEMA_CPP}
    latexDocumentStart.cpp)

target_link_libraries(songbook XercesC::XercesC)

# Run a Python script which extracts and saves a XML schema from xmlSchema.cpp
find_package(Python COMPONENTS Interpreter Development)
if(Python_FOUND)
  set(XML_SCHEMA_XSD ${CMAKE_CURRENT_SOURCE_DIR}/../data/songbookSchema.xsd)
  set(PY_SCRIPT extract_xsd.py)

  add_custom_command(
      OUTPUT ${XML_SCHEMA_XSD}
      COMMAND python ${PY_SCRIPT} ${XML_SCHEMA_CPP} ${XML_SCHEMA_XSD}
      DEPENDS ${XML_SCHEMA_CPP} ${PY_SCRIPT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM
  )

  add_custom_target(extract_xsd
      ALL
      DEPENDS ${XML_SCHEMA_XSD}
  )
else()
  message("Python not found -- XML schema will not be extracted and saved")
endif()