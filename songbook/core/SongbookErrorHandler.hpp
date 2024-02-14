#ifndef SONGBOOK_SONGBOOKERRORHANDLER_HPP
#define SONGBOOK_SONGBOOKERRORHANDLER_HPP

#include <string>
#include <vector>
#include <xercesc/sax/HandlerBase.hpp>

namespace songbook {

    /**
     * Types of errors encountered during XML parsing.
     */
    enum XMLErrorType {warning, error, fatal_error};

    /**
     * Error handler for XML parsing used with `XercesDOMParser`
     */
    class SongbookErrorHandler: public xercesc::HandlerBase {

        public:
        /**
         * Processes an XML parsing warning.
         * 
         * @param e XML parsing warning as an exception
         */
        void warning(const xercesc::SAXParseException& e); 

        /**
         * Processes an XML parsing error. 
         * 
         * @param e XML parsing error as an exception
         */
        void error(const xercesc::SAXParseException& e); 

        /**
         * Processes an XML parsing error.
         * 
         * @param e XML parsing error as an exception
         */
        void fatalError(const xercesc::SAXParseException& e);

        /**
         * `error_occurred` getter.
         * 
         * @return Have errors occurred during XML parsing?
         */
        bool get_error_occurred() const;

        /**
         * Deletes everything from `errors` and sets `error_occurred` data 
         * member to `false`.
         */
        void reset_errors();

        /**
         * Gets string representation of errors which occurred during XML parsing.
         * 
         * @return XML parsing errors
         */
        std::string get_errors() const;

        /**
         * Sets `line_offset`.

         * @param n new value of `line_offset`
         */
        void set_line_offset(int n);

        private:
        /**
         * Extracts information from a `SAXParseException`.
         * 
         * @param e XML parsing warning/error as an exception
         * @return string representation of the exception
         */
        std::string process_exception(const xercesc::SAXParseException& e) const;

        /**
         * Saves an error in the class's data member.
         * 
         * @param type warning/error type
         * @param e associated exception
         */
        void save_error(XMLErrorType type, const xercesc::SAXParseException& e);

        /**
         * Errors which occurred during XML parsing.
         */
        std::vector<std::string> errors;

        /**
         * Has an error occurred (not a warning) during XML parsing?
         */
        bool error_occurred = false;

        /**
         * Number of lines before the currently parsed piece of XML in the
         * original document; used for error messages.
         */
        int line_offset = 0;
    };
}

#endif  // SONGBOOK_SONGBOOKERRORHANDLER_HPP