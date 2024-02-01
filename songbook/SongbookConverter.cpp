#include "SongbookConverter.hpp"
#include "SongbookPrinter.hpp"
#include "SongbookException.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/TransService.hpp>
//#include <xercesc/framework/MemBufInputSource.hpp>


namespace songbook {

    using namespace xercesc;

    LineItem::LineItem(LineItemType type, std::string&& value): 
        type(type), value(std::move(value)) {};

    //------  SongbookConverter member functions ------

    SongbookConverter::SongbookConverter() {

        try {
            XMLPlatformUtils::Initialize();
        } 
        catch (const XMLException& e) {
            char* message = XMLString::transcode(e.getMessage());
            SongbookException ce{"Error during Xerces initialization: ", message};
            XMLString::release(&message);
            throw ce;
        }

        parser = std::make_unique<SongbookParser>();
    }

    /**
     * This is just a string-level extraction of the `<entities>` element
     * at a moment when the document can contain undefined entities.
     */
    ExtractedEntities extract_entities_element(const std::string& xml) {

        size_t start = xml.find("<entities>");
        // return empty map when entities weren't found in the XML
        if (start == std::string::npos) 
            return ExtractedEntities{"", -1};

        std::string end_tag{"</entities>"};
        size_t end = xml.find(end_tag);
        if (end == std::string::npos) 
            throw SongbookException("Error </entities> closing tag not found in XML.");
        
        // return the `<entities>` element and the number of newlines before it
        return ExtractedEntities{
            xml.substr(start, end - start + end_tag.size()),
            static_cast<int>(std::count(cbegin(xml), cbegin(xml) + start, '\n'))};
    }

    void set_language(const std::string& langs) {
        std::string lang;
        const char* loc;

        std::istringstream iss{langs};
        while (getline(iss, lang, ';')) {
            loc = std::setlocale(LC_ALL, (lang + ".utf8").c_str());
            // stop when locale succesfully set ('loc' is not nullptr)
            if (loc) 
                return;
        }

        // when setting locale was not sucessful set at least UTF-8 encoding...
        loc = std::setlocale(LC_ALL, ".utf8");
        // ...or not - when even that is not possible
        if (!loc)
            loc = std::setlocale(LC_ALL, nullptr);
        std::cerr << "Locale could not be set based on supplied language(s): \"" <<
            langs << "\". Using default locale: " << loc << std::endl;
    }

    TagValueMap SongbookConverter::read_entities() const {
        DOMElement* elem = parser->getDocument()->getDocumentElement();
        // first `<entity>` element
        elem = elem->getFirstElementChild();

        TagValueMap entities;
        while (elem) {

            // `<name>` and `<value>` elements (not necessarily in this order)
            DOMElement* first = elem->getFirstElementChild();
            DOMElement* second = first->getNextElementSibling();
            // swap elements when not in the (name, value) order 
            if (get_node_name(first) == "value")
                std::swap(first, second);
            entities.emplace(get_text_value(first), get_text_value(second));

            elem = elem->getNextElementSibling();
        }

        return entities;
    }


    void SongbookConverter::parse_songbook(const std::string& filename) {
        std::string xml = load_xml(filename);

        // extract entities from the XML and parse them
        ExtractedEntities entities = extract_entities_element(xml);
        std::string root;

        if (!entities.xml.empty()) {
            root = "entities";
            insert_dtd(entities.xml, generate_dtd(printer->get_entities(), root), root);
            parser->parse_string(std::move(entities.xml), entities.offset);

            // read entities and update the printer with them
            printer->update_entities(read_entities());
        }

        // parse the songbook using updated entities
        root = "songbook";
        insert_dtd(xml, generate_dtd(printer->get_entities(), root), root);
        parser->parse_string(std::move(xml));
    }

    SongbookConverter::~SongbookConverter() {
        // the pointer must be deleted before calling Terminate()
        parser.reset(nullptr);
        XMLPlatformUtils::Terminate();
    }

    std::string SongbookConverter::convert() {

        DOMElement* root = parser->getDocument()->getDocumentElement();
        DOMElement* elem = root->getFirstElementChild();

        // process settings when present
        if (get_node_name(elem) == "settings") {
            process_settings(elem);
            // proceed to the `<songs>` element
            elem = elem->getNextElementSibling();
        }

        // for storing converted songs
        std::vector<Song> songs;
        if (elem) {
            // proceed to the first `<song>` element
            elem = elem->getFirstElementChild();

            while (elem) {
                songs.push_back(convert_song(elem));
                elem = elem->getNextElementSibling();
            }
        }

        if (sort_songs)
            std::sort(begin(songs), end(songs));

        return printer->print_document(songs);
    }

    void SongbookConverter::process_settings(const DOMElement* settings) {
        DOMElement* elem = settings->getFirstElementChild();
        while (elem) {
            std::string e_name = get_node_name(elem);
            if (e_name == "language")
                set_language(get_text_value(elem));
            if (e_name == "sortSongs")
                sort_songs = (get_text_value(elem) == "yes");
            else if (e_name != "entities") 
                printer->set_parameter(e_name, get_text_value(elem));

            elem = elem->getNextElementSibling();
            
        }
    }

    Song SongbookConverter::convert_song(const DOMNode* song_n) const {
        auto* song_e = dynamic_cast<const DOMElement*>(song_n);

        // process header
        DOMElement* header_e = song_e->getFirstElementChild();
        TagValueMultiMap header_tags = read_song_header(header_e);

        // convert song content
        DOMElement* elem = header_e->getNextElementSibling();
        std::string content = convert_song_content(elem);

        std::string song = printer->print_song(header_tags, content);

        std::string sorting_name;
        auto search = header_tags.find("sortingName");
        if (search != header_tags.end())
            sorting_name = search->second;

        // name must be present
        search = header_tags.find("name");

        return Song{
            search->second,
            std::move(sorting_name),
            std::move(song)};
    }


    TagValueMultiMap SongbookConverter::read_song_header(const DOMElement* header) const {
        TagValueMultiMap tag_values;
        
        DOMElement* elem = header->getFirstElementChild();
        while (elem) {
            std::string name = get_node_name(elem);
            
            if (name == "authors") {  // read authors one by one
                DOMElement* author = elem->getFirstElementChild();
                while (author) {  
                    tag_values.emplace("author", get_text_value(author));
                    author = author->getNextElementSibling();
                }
            } else { // non-author elements
                tag_values.emplace(std::move(name), get_text_value(elem));
            }
            elem = elem->getNextElementSibling();
        }

        return tag_values;
    }


    std::string SongbookConverter::convert_song_content(const DOMElement* content) const {

        std::string result;

        // the given element and all its subsequent siblings
        while (content) {
        
            std::string el_name = get_node_name(content);
            std::string tmp;

            if (el_name == "multicols") {
                tmp = convert_multicols(content);
            } else if (el_name == "line") {
                tmp = convert_line(content);
            } else if (el_name == "columnbreak") {
                tmp = printer->print_columnbreak();
            } else {     // <verse> or <chorus>
                tmp = convert_verse(content, 
                    el_name == "verse" ? VerseType::verse : VerseType::chorus); 
            }
            result.append(tmp);

            content = content->getNextElementSibling();
        }

        return result;
    }

    std::string SongbookConverter::convert_multicols(const DOMElement* multicols) const {
    
        std::string number = get_attr_value(multicols, "number");

        // start multicols, add content, end multicols
        std::string result = printer->print_multicols_start(number);
        result.append(convert_song_content(multicols->getFirstElementChild()));
        result.append(printer->print_multicols_end());
        return result;
    }

    std::string SongbookConverter::convert_verse(const DOMElement* verse,
        VerseType type) const {
    
        // start verse, add content, end verse
        std::string result = printer->print_verse_start(type);
        result.append(convert_song_content(verse->getFirstElementChild()));
        result.append(printer->print_verse_end(type));
        return result;
    }

    std::string SongbookConverter::convert_line(const DOMElement* line) const {
        DOMNode* node = line->getFirstChild();

        std::vector<LineItem> line_content;

        // first use `printer` to create string representations of all elements
        while (node) {
            DOMNode::NodeType type = node->getNodeType();
            if (type == DOMNode::NodeType::TEXT_NODE) {            // lyrics
                std::string lyrics = get_text_value(node);
                // don't include empty lyrics -- might emerge from newline-only
                //   lyrics nodes after newline removal
                if (!lyrics.empty()) {
                    line_content.emplace_back(LineItemType::lyrics, 
                        std::move(lyrics));
                }
            } else if (type == DOMNode::NodeType::ELEMENT_NODE) {  // chord
                TagValueMap chord = read_chord(node);
                line_content.emplace_back(LineItemType::chord, 
                    printer->print_chord(chord));
            }
            node = node->getNextSibling();
        }

        // now print the final line
        std::string result = printer->print_line(line_content);
        
        return result;
    }

    TagValueMap SongbookConverter::read_chord(const DOMNode* chord) const {
        TagValueMap attr_values;
        DOMNamedNodeMap* attrs = chord->getAttributes();
        
        // store all attribute values
        for (XMLSize_t i=0; i < attrs->getLength(); ++i) {
            DOMAttr* attr = dynamic_cast<DOMAttr*>(attrs->item(i));
            std::string a_name = get_node_name(attr);
            std::string a_value = get_value(attr);

            // delete value when "root" is "special"
            if (a_name == "root" && a_value == "special")
                a_value = "";

            attr_values.emplace(std::move(a_name), std::move(a_value));
        }

        // remove "bass" when this chord is "special"
        if (attr_values["root"].empty()) {
            auto bass = attr_values.find("bass");
            if (bass != attr_values.end())
                attr_values.erase(bass);
        }

        return attr_values;
    }


    std::string load_xml(const std::string& filename) {    

        std::ifstream ifs{filename};
        if (!ifs)
            throw std::runtime_error("Input file " + filename + " cannot be opened");

        std::ostringstream oss;
        oss << ifs.rdbuf();
        return std::move(oss).str();
    }

    void insert_dtd(std::string& xml, const std::string& dtd, const std::string& root) {

        if (!dtd.empty()) {
            auto pos = xml.find("<" + root);
            if (pos == std::string::npos) 
                throw SongbookException("<" + root + "> opening tag not found in the XML file");

            xml.insert(pos, dtd);
        }
    }

    std::string get_node_name(const DOMNode* node) {
        if (!node) return "";

        char* name = XMLString::transcode(node->getNodeName());
        std::string result = name;
        XMLString::release(&name);
        return result;
    }

    std::string get_node_name(const DOMAttr* attr) {
        if (!attr) return "";

        char* name = XMLString::transcode(attr->getName());
        std::string result = name;
        XMLString::release(&name);
        return result;
    }

    std::string get_text_value(const DOMNode* node) {
        if (!node || node->getNodeType() != DOMNode::NodeType::TEXT_NODE)    
            return "";

        TranscodeToStr tts(node->getNodeValue(), "utf-8");
        std::string result = reinterpret_cast<char*>(tts.adopt());

        return replace_newlines(std::move(result));
    }

    std::string get_text_value(const DOMElement* elem) {
        DOMNode* child = elem->getFirstChild();

        return get_text_value(child);
    }

    std::string get_value(const DOMAttr* attr) {
        if (!attr)    
            return "";

        TranscodeToStr tts(attr->getValue(), "utf-8");
        return reinterpret_cast<char*>(tts.adopt());
    }

    std::string get_attr_value(const DOMElement* elem, std::string attr_name) {
        XMLCh* attr_name_x = XMLString::transcode(attr_name.c_str());
        DOMAttr* attr = elem->getAttributeNode(attr_name_x);
        XMLString::release(&attr_name_x);

        return get_value(attr);
    }

    std::string generate_dtd(const TagValueMap& entities, const std::string& root) {
        std::string dtd{"<!DOCTYPE " + root + " ["};
        for (const auto& [name, value]: entities) 
            dtd.append("<!ENTITY " + name + " \"" + value + "\">");
        dtd.append("]>");

        return dtd;
    }

    std::string replace_newlines(std::string str, std::string replacement) {
        size_t pos = 0;
        // replace CRLF pairs
        while ((pos = str.find("\r\n", pos)) != std::string::npos) {
            str.replace(pos, 2, replacement);
            ++pos;
        }
        // replace just LF
        pos = 0;
        while ((pos = str.find("\n", pos)) != std::string::npos) {
            str.replace(pos, 1, replacement);
            ++pos;
        }

        return str;
    }
}

