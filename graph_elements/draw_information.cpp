#include "draw_information.hpp"

namespace BuggyDraw
{
//#####################################################################################################################
    std::ostream& DrawInformation::stringify(std::ostream& stream, JSON::StringificationOptions options) const
    {
        std::map <std::string, double> boxMap;
        boxMap["x"] = size.x;
        boxMap["y"] = size.y;
        boxMap["x2"] = size.x2;
        boxMap["y2"] = size.y2;

        options.in_object = true;
        options.ignore_name = false;

        stream << '{';
        JSON::stringify(stream, "size", boxMap, options);
        if (nodeLevel)
            stream << options.delimiter;
        JSON::stringify(stream, "nodeLevel", nodeLevel, options);
        stream << '}';
        return stream;
    }
//---------------------------------------------------------------------------------------------------------------------
    void DrawInformation::parse(std::string const& name, JSON::PropertyTree const& tree, JSON::ParsingOptions const& options)
    {
        auto parseMember = [&](auto& member, std::string const& memberName)
        {
            if (name.empty())
                JSON::parse(member, memberName, tree, options);
            else
                JSON::parse(member, name + "." + memberName, tree, options);
        };
        std::map <std::string, double> boxMap;
        parseMember(boxMap, "size");
        parseMember(nodeLevel, "nodeLevel");

        size.x = boxMap["x"];
        size.y = boxMap["y"];
        size.x2 = boxMap["x2"];
        size.y2 = boxMap["y2"];
    }
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
}
