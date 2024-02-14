#include "SongbookErrorHandler.hpp"

#include <sstream>
#include <xercesc/util/XMLString.hpp>

namespace songbook {

    using namespace xercesc;

    void SongbookErrorHandler::warning(const xercesc::SAXParseException& e) {
        save_error(XMLErrorType::warning, e);
    }

    void SongbookErrorHandler::error(const SAXParseException& e) {
        save_error(XMLErrorType::error, e);
    }

    void SongbookErrorHandler::fatalError(const SAXParseException& e) {
        save_error(XMLErrorType::fatal_error, e);
    }

    bool SongbookErrorHandler::get_error_occurred() const {
        return error_occurred;
    }

    void SongbookErrorHandler::reset_errors() {
        errors.clear();
        error_occurred = false;
    }

    std::string SongbookErrorHandler::get_errors() const {
        std::string errors_str;
        for (auto e: errors) {
            errors_str += e;
            errors_str += "\n";
        }
        // remove last newline if there were any errors
        if (errors_str.size() > 0)
            errors_str.pop_back();
    
        return errors_str;
    }

    void SongbookErrorHandler::set_line_offset(int n) {
        line_offset = n;
    }

    void SongbookErrorHandler::save_error(XMLErrorType type, const xercesc::SAXParseException& e) {
        std::string message;
        switch (type) {
            case XMLErrorType::warning:     message = "Warning"; 
                break;
            case XMLErrorType::error:       message = "Error"; 
                error_occurred = true;
                break;
            case XMLErrorType::fatal_error: message = "Fatal error";
                error_occurred = true;
        }
        message += ": ";
        message += process_exception(e);

        errors.emplace_back(std::move(message));
    }

    std::string SongbookErrorHandler::process_exception(const SAXParseException& e) const {
        std::ostringstream oss;
        char* message = XMLString::transcode(e.getMessage());
        oss << "line " << e.getLineNumber() + line_offset << 
            ", column " << e.getColumnNumber() << ": " << message;
        XMLString::release(&message);
        return oss.str();
    }
}