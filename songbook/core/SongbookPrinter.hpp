#ifndef SONGBOOK_SONGBOOKPRINTER_HPP
#define SONGBOOK_SONGBOOKPRINTER_HPP

#include <string>
#include <vector>

#include "songbookTypes.hpp"
#include "Song.hpp"

namespace songbook {

    /**
     * Base class for a printer used by `SongbookConverter`. Provides only basic
     * conversion to string. *Printing* in the context of this class refers to 
     * creating a string representation.
     */
    class SongbookPrinter {

        public:
        /**
         * Default constructor.
         */
        SongbookPrinter();

        /**
         * Default virtual destructor.
         */
        virtual ~SongbookPrinter() = default;

        /**
         * Sets parameter value. Overwrites an existing one or creates a new one.
         * 
         * @param name parameter name
         * @param value parameter value
         */
        void set_parameter(std::string name, std::string value);

        /**
         * Retrieves value of a parameter.
         * 
         * @param name parameter name
         * @return value of the parameter
         */
        std::string get_parameter(const std::string& name) const;

        /**
         * Getter for `entities`.
         * 
         * @return `entities` name-value pairs
         */
        TagValueMap get_entities() const;
        
        /**
         * Updates `entities` with supplied values. Values of already existing 
         * entities are overwritten, new are created.
         * 
         * @param ent_update entity name-value pairs
         */
        void update_entities(TagValueMap ent_update);

        /**
         * Prints the beginning of a document of the target format.
         *
         * @return empty string
         */
        virtual std::string print_document_start() const;

        /**
         * Prints document end.
         * 
         * @return empty string
         */
        virtual std::string print_document_end() const;

        /**
         * Prints start of the multicolumn environment.
         * 
         * @param number number of columns
         * @return empty string
         */
        virtual std::string print_multicols_start(const std::string& number) const;

        /**
         * Prints a columnbreak for a multicolumn environment.
         * 
         * @return empty string
         */
        virtual std::string print_columnbreak() const;

        /**
         * Prints the end of the multicolumn environment.
         * 
         * @return empty string
         */
        virtual std::string print_multicols_end() const;

        /**
         * Prints the beginning of a verse of given type.
         * 
         * @param type verse type
         * @return empty string
         */
        virtual std::string print_verse_start(VerseType type) const;

        /**
         * Prints the end of a verse of given type.
         * 
         * @param type verse type
         * @return empty string
         */
        virtual std::string print_verse_end(VerseType type) const;

        /**
         * Prints song header.
         * 
         * @param tag_values tag-value pairs for song header properties
         * @return song header
         */
        virtual std::string print_song_header(const TagValueMultiMap& tag_values) const;

        /**
         * Prints song end.
         * 
         * @return a string containing just a new line
         */
        virtual std::string print_song_end() const;

        /**
         * Prints one chord.
         * 
         * @param chord chord attributes' name-value pairs
         * @return simple string representation of a chord
         */
        virtual std::string print_chord(const TagValueMap& chord) const;

        /**
         * Prints the whole line.
         * 
         * @param line_content line items
         * @return song line witch chords in brackets
         */
        virtual std::string print_line(const std::vector<LineItem>& line_content) const;

        /**
         * Prints the whole song.
         * 
         * @param header song header attributes' name-value pairs
         * @param content already printed song content
         * @return concatenated song header, content and song end
         */
        virtual std::string print_song(const TagValueMultiMap& header, 
            const std::string& content) const;

        /**
         * Prints the whole document
         * 
         * @param songs individual already converted songs
         * @return concatenation of document start, songs and document end
         */
        virtual std::string print_document(const std::vector<Song>& songs) const;

        // ----- data members -----
        protected:
        /**
         * Name-value pairs for parameters used for printing.
         */
        TagValueMap parameters;

        /**
         * Name-value pairs for entity translation.
         */
        TagValueMap entities;
    };
}

#endif  // SONGBOOK_SONGBOOKPRINTER_HPP
