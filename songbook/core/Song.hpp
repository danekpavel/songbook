#ifndef SONGBOOK_SONG_HPP
#define SONGBOOK_SONG_HPP

#include <string>
#include <sstream>

namespace songbook {

    /**
     * Contains information about a song and its content.
     * 
     */
    class Song {

        public:
        /**
         * Default constructor.
         */
        Song() = default;

        /**
         * Constructor.
         * 
         * @param name song name
         * @param sorting_name an alternative song name used for sorting
         * @param content song content including header
         */
        Song(std::string name, std::string sorting_name, std::string content);

        /**
         * Name setter.
         * 
         * @param n new song name
         */
        void set_name(const std::string& n);

        /**
         * Getter for `name`.
         * 
         * @return song name
         */
        std::string get_name() const;

        /**
         * Getter for `content`.
         * 
         * @return song content
         */
        std::string get_content() const;

        /**
         * Returns the song name to be used for sorting.
         * 
         * @return `sorting_name` when not empty, `name` otherwise
         */
        std::string get_sorting_name() const;

        /**
         * Appends a string to song content.
         * 
         * @param str string to append
         */
        void append_content(const std::string& str);

        private:
        std::string name;           ///< song name
        std::string sorting_name;   ///< song name used for sorting
        std::string content;        ///< song content
    };

    /**
     * Compares two songs based on their sorting name using current locale.
     * 
     * @param lhs first song
     * @param rhs second song
     * @return `true` if `lhs` < `rhs`, `false` otherwise
     */
    bool operator <(const Song& lhs, const Song& rhs);
}
#endif  // SONGBOOK_SONG_HPP
