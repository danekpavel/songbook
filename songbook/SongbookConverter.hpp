#ifndef SONGBOOK_SONGBOOKCONVERTER_HPP
#define SONGBOOK_SONGBOOKCONVERTER_HPP

#include "songbookTypes.hpp"
#include "SongbookParser.hpp"
#include "SongbookPrinter.hpp"
#include "Song.hpp"

#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <map>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>

namespace songbook {

    /**
     * Converts a XML songbook file into other format, depending on the 
     * specified `SongbookPrinter`-derived class. 
     * 
     * An example usecase may look like this: initialize a new `SongbookConvertor`
     * using a subclass of `SongbookPrinter`, use it to parse an XML
     * file and than to convert it to the desired format:
     * @code
     * SongbookConverter converter = init_converter<SongbookPrinterLatex>();
     * converter.parse_songbook("sb.xml");
     * std::string output = converter.convert();
     * @endcode
     */
    class SongbookConverter {

        public:
        /**
         * Constructor which does all necessary work before an XML can be parsed. 
         * 
         * Initializes `XMLPlatformUtils` and creates the parser.
         * 
         * @throws ConverterException failed Xerces initialization
         */
        SongbookConverter();

        /**
         * Copy constructor not available.
         * 
         * @param other 
         */
        SongbookConverter(const SongbookConverter& other) = delete;
        
        /**
         * Default move constructor.
         * 
         * @param other other object
         */
        SongbookConverter(SongbookConverter&& other) = default;

        /**
         * Assignment operator not available
         * 
         * @param other 
         * @return 
         */
        SongbookConverter& operator=(const SongbookConverter& other) = delete;

        /**
         * Default move assignment operator.
         * 
         * @param other other object
         * @return assigned object
         */
        SongbookConverter& operator=(SongbookConverter&& other) = default;

        /**
         * Destructor `Terminate()`s the parser.
         */
        ~SongbookConverter();

        /**
         * Converts parsed XML into final format using the `printer`.
         * 
         * @return converted songbook
         */
        std::string convert();

        /**
         * Parses a songbook XML read from a file. 
         * 
         * Before parsing, a DTD with entity definitions from the `printer`
         * is inserted.
         * 
         * @param filename path to the songbook XML file
         * @throws ConvertorException a problem during XML parsing
         */
        void parse_songbook(const std::string& filename);

        /**
         * Constructs and sets the `printer`.
         * 
         * @tparam T printer class derived from `SongbookPrinter`
         */
        template <typename T> void set_printer();

        private:

        /**
         * Reads entity definitions after the `<entities>` element
         * has been parsed.
         * 
         * @return entity name-value pairs
         */
        TagValueMap read_entities() const;

        /**
         * Processes settings from the XML file and passes them to the `printer`
         * to save them. `<entities>` element is ignored.
         * 
         * @param settings `<settings>` XML element
         */
        void process_settings(const xercesc::DOMElement* settings);

        /**
         * Reads information from a song header.
         * 
         * @param header `<header>` XML element
         * @return element-value pairs from the header
         */
        TagValueMultiMap read_song_header(const xercesc::DOMElement* header) const;

        /**
         * Reads information from a chord.
         * 
         * @param chord `<chord>` XML element
         * @return attribute-value pairs for the chord
         */
        TagValueMap read_chord(const xercesc::DOMNode* chord) const;

        /**
         * Converts a `<song>` element into a `Song` object
         * 
         * @param song_n `<song>` XML element
         * @return converted song
         */
        Song convert_song(const xercesc::DOMNode* song_n) const;

        /**
         * Converts content of (a part of) a song. Starts with the given XML 
         * element and continues with all its subsequent siblings. Typically
         * will be called recursively, not directly though.
         * 
         * @param content song content element to start from
         * @return converted song content
         */
        std::string convert_song_content(const xercesc::DOMElement* content) const;

        /**
         * Converts the `<multicols>` XML element.
         * @param multicols `<multicols>` XML element
         * @return converted song content
         */
        std::string convert_multicols(const xercesc::DOMElement* multicols) const;

        /**
         * Converts a verse XML element (`<verse>` or 
         * `<chorus>`).
         * 
         * @param verse verse XML element
         * @param type verse type (verse or chorus)
         * @return converted verse
         */
        std::string convert_verse(const xercesc::DOMElement* verse, VerseType type) const;
            
        /**
         * Converts a `<line>` XML element.
         * @param line `<line>` XML element
         * @return converted line element
         */
        std::string convert_line(const xercesc::DOMElement* line) const;

        // data members
        private:
        /**
         * XML parser.
         */
        std::unique_ptr<SongbookParser> parser;

        /**
         * Printer used for creating the final document.
         */
        std::unique_ptr<SongbookPrinter> printer;

        /**
         * Should songs be sorted by name?
         */
        bool sort_songs = true;
    };


    /**
     * Creates a new `SongbookConvertor` with a printer of 
     * specified class.
     * 
     * @tparam T `SongbookPrinter`-derived class
     * @return new converter
     */
    template <typename T> SongbookConverter init_converter();

    /**
     * Reads an XML from a file.
     * 
     * @param filename file to read from
     * @return XML file content
     * @throw std::runtime_error file cannot be opened
     */
    std::string load_xml(const std::string& filename); 

    /**
     * Inserts a DTD into a songbook XML. 
     * 
     * Not very sophisticated, just inserts the DTD before the document's root 
     * element. If the XML already contained
     * a DTD, the resulting document would not be valid and subsequent XML 
     * parsing would fail.
     * 
     * @param xml songbook XML document
     * @param dtd DTD string; nothing happens when empty
     * @param root name of the root element
     * @throw ConverterException when the opening tag is not found
     */
    void insert_dtd(std::string& xml, const std::string& dtd, const std::string& root);
    std::string get_node_name(const xercesc::DOMNode* node);

    /**
     * Retrieves text content from an XML node.
     * 
     * @param node 
     * @return 
     */
    std::string get_text_value(const xercesc::DOMNode* node);
    std::string get_text_value(const xercesc::DOMElement* elem);

    std::string get_value(const xercesc::DOMAttr* attr);

    /**
     * Retrieves the value of an XML element's attribute specified by name.
     * 
     * @param elem XML element
     * @param attr_name attribute name
     * @return value of the attribute (empty string when not specified or default)
     */
    std::string get_attr_value(const xercesc::DOMElement* elem, std::string attr_name);

    /**
     * Creates a DTD with entity definitions.
     * 
     * @param entities entities' name-value pairs
     * @param root name of XML root element
     * @return DTD with entity definitions
     */
    std::string generate_dtd(const TagValueMap& entities, const std::string& root);

    /**
     * Changes all newlines in a string to something else (empty string by default)
     * 
     * @param str string where newlines will be replaced
     * @param replacement replacement for a newline
     * @return string with newlines replaced
     */
    std::string replace_newlines(std::string str, std::string replacement = "");

    /**
     * Extracts the `<entities>` element (and its content) from an XML. Just
     * string-based, no parsing. Element will be extracted even if it is inside
     * a comment.
     * 
     * @param xml XML document
     * @return struct with `xml` containing the `<entities>` element and `line`
     * containing the line on which it begins in the XML; `line` is -1 when
     * the element is not present
     * @throw ConverterException when the `<entities>` element starts but 
     * its content cannot be parsed
     */
    ExtractedEntities extract_entities_element(const std::string& xml);

    /**
     * Sets locale (LC_ALL) to the given language/language-location combination.
     * 
     * Tries all semicolon-separated possibilities from `langs` until first 
     * success. In case of no succes, a message including current locale is 
     * written to std::cerr.
     * 
     * @param langs semicolon-separated list of locale candidates
     */
    void set_language(const std::string& langs);

    
    template <typename T>
    void SongbookConverter::set_printer() {
        printer = std::make_unique<T>();
    }

    template <typename T>
    SongbookConverter init_converter() {
        SongbookConverter converter{};
        converter.set_printer<T>();

        return converter;
    }
}

#endif  // SONGBOOK_SONGBOOKCONVERTER_HPP

