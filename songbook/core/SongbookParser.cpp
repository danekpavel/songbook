#include "SongbookParser.hpp"
#include "SongbookException.hpp"

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/validators/schema/SchemaGrammar.hpp>

namespace songbook {

    using namespace xercesc;

    SongbookParser::SongbookParser() {
    
        setDoNamespaces(true);
        useCachedGrammarInParse(true);
        setDoSchema(true);
        setValidationScheme(XercesDOMParser::Val_Auto);
        // XML entities are replaced with text
        setCreateEntityReferenceNodes(false);
        // validation errors will not stop parsing; however, if they occur
        //   they will prevent converting the document later on, after it is parsed
        setValidationConstraintFatal(false);
        setExitOnFirstFatalError(true);

        // loading XML schema read from the global xml_schema variable 
        MemBufInputSource schema_buf(
            reinterpret_cast<const XMLByte*>(xml_schema.c_str()), 
            xml_schema.length(), 
            "xsd", 
            false);
        loadGrammar(schema_buf, Grammar::SchemaGrammarType, true);

        error_handler = std::make_unique<SongbookErrorHandler>();
        setErrorHandler(error_handler.get());
    }

    void SongbookParser::parse_string(std::string xml, int offset) {

        try {
            MemBufInputSource xml_buf(
                reinterpret_cast<const XMLByte*>(xml.c_str()), 
                xml.length(), 
                "xml", 
                false);

            // remove errors from previous parsing
            error_handler->reset_errors();
            error_handler->set_line_offset(offset);
            parse(xml_buf);

            if (error_handler->get_error_occurred()) {
                throw SongbookException(//"Error(s) occurred during XML parsing:\n",
                    error_handler->get_errors());
            }
        }
        catch (const SongbookException& se) {
            throw;
        }
        catch (const XMLException& e) {
            char* message = XMLString::transcode(e.getMessage());
            SongbookException se{"Error during XML parsing: ", message};
            XMLString::release(&message);
            throw se;
        }
        catch (const DOMException& e) {
            char* message = XMLString::transcode(e.msg);
            SongbookException se{"Error during XML parsing: ", message};
            XMLString::release(&message);
            throw se;
        }
        catch (...) {
            SongbookException se{"Unexpected error during XML parsing!"};
            throw se;
        }
    }
}
