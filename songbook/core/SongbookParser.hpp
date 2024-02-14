#ifndef SONGBOOK_SONGBOOKPARSER_HPP
#define SONGBOOK_SONGBOOKPARSER_HPP

#include "SongbookErrorHandler.hpp"

#include <memory>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>


namespace songbook {

    /**
     * `xml_schema` defined in its own .cpp file.
     */
    extern std::string xml_schema;

    /**
     * An extension of `xercesc::XercesDOMParser` with its own error handler.
     */
    class SongbookParser: public xercesc::XercesDOMParser {

        public:
        /**
         * Default constructor.
         * 
         * Sets up the parser, loads XML schema and creates the error handler.
         */
        SongbookParser();

        /**
         * Parses XML from a string.
         * 
         * @param xml an XML
         * @param offset line on which `xml` started in the original document
         * @throws SongbookException a problem during XML parsing
         */
        void parse_string(std::string xml, int offset = 0);

        private:

        /**
         * Error handler used by the parser.
         */
        std::unique_ptr<SongbookErrorHandler> error_handler;
    };
}
#endif  // SONGBOOK_SONGBOOKPARSER_HPP
