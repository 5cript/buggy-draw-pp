#pragma once

#include "cairo-wrap/bounding_box.hpp"
#include "../json_convert.hpp"

namespace BuggyDraw
{
    struct DrawInformation
    {
        Cairo::BoundingBox size;
        boost::optional <int> nodeLevel;
        double yLayerStart;
        double xLayerStart;

        std::ostream& stringify(std::ostream& stream, JSON::StringificationOptions options) const;
        void parse(std::string const& name, JSON::PropertyTree const& tree, JSON::ParsingOptions const& options = {});
    };
}
