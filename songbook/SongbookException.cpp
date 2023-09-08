#include "SongbookException.hpp"

namespace songbook {

    SongbookException::SongbookException(const char* msg): 
        std::runtime_error(msg) {}

    SongbookException::SongbookException(const std::string& msg): 
        std::runtime_error(msg) {}

    SongbookException::SongbookException(const std::string& title, 
        const char* msg): 
        std::runtime_error(title + msg) {}

    SongbookException::SongbookException(const std::string& title, 
        const std::string& msg): 
        SongbookException(title, msg.c_str()) {}

}