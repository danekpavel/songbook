#include "SongbookPrinterLatex.hpp"

namespace songbook {

    SongbookPrinterLatex::SongbookPrinterLatex() {

        parameters.emplace("mainFont", "Linux Libertine O");
        parameters.emplace("chordFont", "Calibri");

        // redefine entities
        TagValueMap ent{
            {"nbsp", "~"},
            {"ndash", "--"},
            {"mdash", "---"},
            {"hellip", R"(\ldots )"},
            {"ampersand", R"(\&amp;)"},
            {"quoteEnglishOpen", "``"},
            {"quoteEnglishClose", "''"},
            {"quoteCzechOpen", R"(\quotedblbase )"},
            {"quoteCzechClose", R"(\textquotedblleft )"},
            {"thinsp", R"(\thinspace )"},
            {"times", R"($\times$)"}
        };
        update_entities(ent);
    }

    std::string SongbookPrinterLatex::print_document_start() const {
        std::string doc_start{latex_document_start};

        for (auto [name, value]: parameters)
            replace_parameter(doc_start, name, value);

        return doc_start;
    }

    std::string SongbookPrinterLatex::print_document_end() const {
        return "\n\\end{document}";
    }

    std::string SongbookPrinterLatex::print_multicols_start(const std::string& number) const {
        return "\\begin{multicols}{" + number + "}\\raggedcolumns\n";
    }

    std::string SongbookPrinterLatex::print_columnbreak() const {
        return "\\columnbreak\n";
    }

    std::string SongbookPrinterLatex::print_multicols_end() const {
        return "\\end{multicols}\n";
    }

    std::string SongbookPrinterLatex::print_verse_start(VerseType type) const {
        if (type == VerseType::verse)
            return "\\verse{";
        else
            return "\\chorus{";
    }

    std::string SongbookPrinterLatex::print_verse_end(VerseType type) const {
        return "}\n\n";
    }

    std::string SongbookPrinterLatex::print_song_header(const TagValueMultiMap& tag_values) const {
        std::string song_name;
        std::string left;      // left-aligned header content
        std::string right;     // right-aligned ...

        for (auto [tag, value]: tag_values) {
            if (tag == "name")
                song_name = value;
            else if (tag == "author")  // separate authors with " / "
                left = left.empty() ? value : left + " / " + value;
            else if (tag == "album")   // e.g., "1973" or "The Wall (1973)"
                right = right.empty() ? value : value + " (" + right + ")";
            else if (tag == "year")    // e.g., "The Wall" or "The Wall (1973)"
                right = right.empty() ? value : right + " (" + value + ")";
        }
        std::string header{"\n\\song{" + song_name + "}{" + left +
            "}{" + right + "}\n\n"};

        return header;
    }

    std::string SongbookPrinterLatex::print_line(const std::vector<LineItem>& line_content) const {

        std::string line = "\\sbline{";
        std::string chords;
        int n_lyrics{0};

        // count the number of lyrics items to later know if a lyrics item is
        //   the last one
        for (auto lc: line_content) 
            if (lc.type==LineItemType::lyrics) 
                ++n_lyrics;

        int i_lyrics{0};
        for (auto lc: line_content) {
            if (lc.type==LineItemType::lyrics) {
                ++i_lyrics;
                // put together with previously read chord(s)
                if (!chords.empty()) {  
                    line.append("\\chordslyrics");
                    // use "\chordslyricshyphen" when the current lyrics aren't
                    //   the last on this line and don't end with a space
                    if (i_lyrics < n_lyrics && lc.value.back() != ' ')
                        line.append("hyphen");
                    line.append("{" + chords + "}{" + lc.value + "}");
                    chords = "";
                // just lyrics
                } else               
                    line.append(lc.value);
            } else  // LineItemType::chord
                chords.append(lc.value); 
        }

        // when trailing chords (without associated lyrics) present
        if (!chords.empty())   
            if (n_lyrics > 0)  // mixed content line
                line.append("\\chordslyrics{" + chords + "}{}");
            else               // chords only line
                line.append(chords);
        
        return line.append("}\n");
    }

    std::string SongbookPrinterLatex::print_chord(const TagValueMap& chord) const {

        auto attr = chord.find("root");
        std::string result = replace_flat_sharp_latex(attr->second);

        attr = chord.find("type");
        if (attr != chord.end())
            result.append(attr->second);

        attr = chord.find("bass");
        if (attr != chord.end())
            result.append("/" + replace_flat_sharp_latex(attr->second));

        attr = chord.find("optional");
        if (attr != chord.end() && attr->second == "yes")
            result = "(" + result + ")";
        
        return "\\chord{" + result +"}";
    }

    bool replace_parameter(std::string& str, const std::string& name, 
        const std::string& value) {

        std::string pattern = "@@@" + name + "@@@";
        auto pos = str.find(pattern);
        if (pos == std::string::npos)
            return false;

        str.replace(pos, pattern.length(), value);
        return true;
    }

    std::string replace_flat_sharp_latex(std::string note) {
        // replace sharp
        auto pos = note.find('#');
        if (pos != std::string::npos)
            note.replace(pos, 1, "\\msharp ");
        else {
            // replace flat
            pos = note.find('b', 1);
            if (pos != std::string::npos)
                note.replace(pos, 1, "$\\flat$");
        }
        
        return note;
    }
}