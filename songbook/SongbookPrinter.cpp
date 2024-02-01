#include "SongbookPrinter.hpp"

#include <string>
#include <iostream>

namespace songbook {

    SongbookPrinter::SongbookPrinter() {
        // default parameters
        parameters.emplace("chorusLabel", "Ref");
        parameters.emplace("language", "cs");
        parameters.emplace("tocTitle", "Obsah");

        // default entity values
        entities.emplace("nbsp", " ");
        entities.emplace("ndash", "--");
        entities.emplace("mdash", "---");
        entities.emplace("hellip", "...");
        entities.emplace("ampersand", "&amp;");
        entities.emplace("quoteEnglishOpen", "&quot;");
        entities.emplace("quoteEnglishClose", "&quot;");
        entities.emplace("quoteCzechOpen", "„");
        entities.emplace("quoteCzechClose", "“");
        entities.emplace("thinsp", " ");
        entities.emplace("times", "x");
    }

    void SongbookPrinter::set_parameter(std::string name, std::string value) {
        parameters[name] = value;
    }

    std::string SongbookPrinter::get_parameter(const std::string &name) const {
        auto it = parameters.find(name);
        if (it != parameters.end())
            return it->second;

        return "";
    }

    TagValueMap SongbookPrinter::get_entities() const {
        return entities;
    }

    void SongbookPrinter::update_entities(TagValueMap ent_update) {
        for (const auto& [value, name]: ent_update)
            entities[value] = name;
    }

    std::string SongbookPrinter::print_columnbreak() const {
        return "";
    }

    std::string SongbookPrinter::print_song_end() const {
        return "\n";
    }

    std::string SongbookPrinter::print_document_start() const {
        return "";
    }

    std::string SongbookPrinter::print_document_end() const {
        return "";
    }

    std::string SongbookPrinter::print_multicols_start(const std::string& number) const {
        return "";
    }

    std::string SongbookPrinter::print_multicols_end() const {
        return "";
    }

    std::string SongbookPrinter::print_verse_start(VerseType type) const {
        if (type == VerseType::verse)
            return "";
        else
            return get_parameter("chorusLabel") + ":\n";
    }

    std::string SongbookPrinter::print_verse_end(VerseType type) const {
        return "\n";
    }

    std::string SongbookPrinter::print_song_header(const TagValueMultiMap& tag_values) const {
        auto it = tag_values.find("name");
        std::string song_name{it->second};

        // start with [name] and then put all other [tag]: [value] pairs
        std::string header_content{song_name};
        header_content += '\n';
        for (const auto& [tag, value] : tag_values) {
            if (tag != "name")
                header_content.append(tag + ": " + value + '\n');
        }
        header_content += '\n';

        return header_content;
    }

    std::string SongbookPrinter::print_line(const std::vector<LineItem>& line_content) const {
        
        std::string line;

        for (const auto& lc : line_content) {
            if (lc.type==LineItemType::lyrics) 
                line.append(lc.value);
            else  // chord
                line.append("[" + lc.value + "]"); 
        }
        
        return line.append("\n");
    }

    std::string SongbookPrinter::print_document(const std::vector<Song> &songs) const {

        std::string result = print_document_start();

        for (const auto& song : songs) 
            result.append(song.get_content());

        result.append(print_document_end());

        return result;
    }

    std::string SongbookPrinter::print_song(const TagValueMultiMap& header_tags, 
        const std::string& content) const {

        std::string song = print_song_header(header_tags);
        song.append(content);
        song.append(print_song_end());

        return song;
    }

    std::string SongbookPrinter::print_chord(const TagValueMap& chord) const {

        auto attr = chord.find("root");
        std::string result{attr->second};

        attr = chord.find("type");
        if (attr != chord.end())
            result.append(attr->second);

        attr = chord.find("bass");
        if (attr != chord.end())
            result.append("/" + attr->second);

        attr = chord.find("optional");
        if (attr != chord.end() && attr->second == "yes")
            result = "(" + result + ")";
        
        return result;
    }
}