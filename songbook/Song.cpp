#include "Song.hpp"

#include <cstring>


namespace songbook {

    Song::Song(std::string name, std::string sorting_name, std::string content): 
        name(std::move(name)), 
        sorting_name(std::move(sorting_name)), 
        content(std::move(content)) {}
    
    void Song::set_name(const std::string& n) {
        name = n;
    }

    std::string Song::get_name() const
    {
        return name;
    }

    void Song::append_content(const std::string& str) {
        content += str;
    }

    std::string Song::get_content() const {
        return content;
    }

    std::string Song::get_sorting_name() const {

        if (sorting_name == "")
            return name;
        else 
            return sorting_name;
    }

    bool operator <(const Song &lhs, const Song &rhs) {

        return strcoll(lhs.get_sorting_name().c_str(), rhs.get_sorting_name().c_str()) < 0;
    }
}