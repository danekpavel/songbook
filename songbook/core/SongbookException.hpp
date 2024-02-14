#ifndef SONGBOOK_SONGBOOKEXCEPTION_HPP
#define SONGBOOK_SONGBOOKEXCEPTION_HPP

#include <stdexcept>

namespace songbook {

    /**
     * Custom exception class.
     */
    class SongbookException: public std::runtime_error {
        public:
        /**
         * Constructor.
         * 
         * @param msg error message
         */
        SongbookException(const char* msg);

        /**
         * Constructor.
         * 
         * @param msg error message
         */
        SongbookException(const std::string& msg);

        /**
         * Constructor which combines two strings into one message.
         * 
         * @param title first messsage part
         * @param msg second message part
         */
        SongbookException(const std::string& title, const char* msg);

        /**
         * Constructor which combines two strings into one message.
         * 
         * @param title first messsage part
         * @param msg second message part
         */
        SongbookException(const std::string& title, const std::string& msg); 
    };
}

#endif  // SONGBOOK_SONGBOOKEXCEPTION_HPP