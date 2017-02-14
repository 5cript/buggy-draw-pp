#pragma once

#include "../json_convert.hpp"

namespace BuggyDraw
{
    struct Settings : public JSON::Stringifiable <Settings>
                    , public JSON::Parsable <Settings>
    {
        boost::optional <bool> isRecursive;
    };
}

BOOST_FUSION_ADAPT_STRUCT
(
    BuggyDraw::Settings,
    (boost::optional <bool>, isRecursive)
)
