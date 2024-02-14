#ifndef SONGBOOK_SONGBOOKPRINTERLATEX_HPP
#define SONGBOOK_SONGBOOKPRINTERLATEX_HPP

#include "SongbookPrinter.hpp"

namespace songbook {

    extern std::string latex_document_start;

    /**
     * A `SongbookPrinter` which supplies methods for printing a LaTeX document
     */
    class SongbookPrinterLatex: public SongbookPrinter {

        public:
        SongbookPrinterLatex();

        /**
         * @copybrief SongbookPrinter::print_document_start()
         * 
         * Includes the LaTeX document preamble with all necessary definitions,
         * beginning of document body and inserts table of contents.
         * 
         * @return start of the LaTeX document
         */
        std::string print_document_start() const override;

        /**
         * @copybrief SongbookPrinter::print_document_end()
         * 
         * @return end of the LaTeX document
         */
        std::string print_document_end() const override;

        /**
         * @copybrief SongbookPrinter::print_multicols_start()
         * 
         * @param number number of columns 
         * @return beginning of the LaTeX `multicols` environment
         */
        std::string print_multicols_start(const std::string& number) const override;

        /**
         * @copybrief SongbookPrinter::print_columnbreak()
         * 
         * @return `\columnbreak`
         */
        std::string print_columnbreak() const override;

        /**
         * @copybrief SongbookPrinter::print_multicols_end()
         * 
         * @return end of `multicols` environment
         */
        std::string print_multicols_end() const override;

        /**
         * @copybrief SongbookPrinter::print_verse_start()
         * 
         * @param type verse type
         * @return start of `\verse` or `\chorus` command
         */
        std::string print_verse_start(VerseType type) const override;

        /**
         * @copybrief SongbookPrinter::print_verse_end()
         * 
         * @param type 
         * @return verse command closing bracket
         */
        std::string print_verse_end(VerseType type) const override;

        /**
         * @copybrief SongbookPrinter::print_song_header()
         * 
         * @param tag_values tag-value pairs for song header properties
         * @return `\song` command
         */
        std::string print_song_header(const TagValueMultiMap& tag_values) const override;

        /**
         * @copybrief SongbookPrinter::print_line()
         * 
         * Combines line items into a line using the `\chordslyrics`
         * and `\chordslyricshyphen` LaTeX commands where appropriate.
         * 
         * @param line_content line items
         * @return complete `\sbline` command for one line
         */
        std::string print_line(const std::vector<LineItem>& line_content) const override; 

        /**
         * @copybrief SongbookPrinter::print_chord()
         * 
         * @param chord chord attributes' name-value pairs
         * @return `\chord` command for one chord
         */
        std::string print_chord(const TagValueMap& chord) const override;
    };


    // ---- Nonmember functions ------

    /**
     * Replaces the first occurrence of a parameter marked as `@@@name@@@` with 
     * its value inside a string.
     * 
     * @param str string where the parameter is replaced
     * @param name parameter name
     * @param value parameter value
     * @return `true` when the parameter was successfully replaced, `false`
     * otherwise 
     */
    bool replace_parameter(std::string& str, const std::string& name, const std::string& value);

    /**
     * Replaces a sharp (#) or a flat (b) musical notation with its LaTeX
     * representation.
     * 
     * @param note musical note
     * @return note with LaTeX sharp or flat
     */
    std::string replace_flat_sharp_latex(std::string note);
}

#endif  // SONGBOOK_SONGBOOKPRINTERLATEX_HPP
