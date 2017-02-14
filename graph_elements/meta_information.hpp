#pragma once

#include "../json_convert.hpp"

namespace BuggyDraw
{
    struct TypeInformation : public JSON::Stringifiable <TypeInformation>
                           , public JSON::Parsable <TypeInformation>
    {

    };

    struct MetaInformation : public JSON::Stringifiable <MetaInformation>
                           , public JSON::Parsable <MetaInformation>
    {
        boost::optional <float> value;
        boost::optional <std::string> type;
        boost::optional <TypeInformation> typeInformation;

    };
}

BOOST_FUSION_ADAPT_STRUCT
(
    BuggyDraw::TypeInformation,
)

BOOST_FUSION_ADAPT_STRUCT
(
    BuggyDraw::MetaInformation,
    (boost::optional <float>, value)
    (boost::optional <std::string>, type)
    (boost::optional <TypeInformation>, typeInformation)
)
