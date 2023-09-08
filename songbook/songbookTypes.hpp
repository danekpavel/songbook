/**
 * @file
 * 
 * Declarations of data types.
*/

#ifndef SONGBOOK_SONGBOOKTYPES_HPP
#define SONGBOOK_SONGBOOKTYPES_HPP

#include <map>
#include <string>

namespace songbook {
    /**
     * A `std::map` for storing tag-value pairs.
     */
    using TagValueMap = std::map<std::string, std::string>;

    /**
     * A `std::multimap` for storing tag-value pairs.
     */
    using TagValueMultiMap = std::multimap<std::string, std::string>;

    /**
     * Specifies whether a line item is lyrics (text) or a chord.
     */
    enum LineItemType {lyrics, chord};

    /**
     * Specifies verse type.
     */
    enum VerseType {verse, chorus};

    /**
     * Lyrics or chord line item structure.
     */
    struct LineItem {
        LineItemType type;  /**< type of the item */
        std::string value;  /**< item's value*/
        /**
         * Constructor
         * 
         * @param type line item type
         * @param value value of the item
         */
        LineItem(LineItemType type, std::string&& value);
    };

    /**
     * Stores the `<entities>` element and the number of lines before it
     */
    struct ExtractedEntities {
        std::string xml;  /**< The `<entities>` element and its content */
        int offset;       /**< Number of lines before the `<entities>` element */
    };
}

#endif  // SONGBOOK_SONGBOOKTYPES_HPP