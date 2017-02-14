#pragma once

#include "../json_convert.hpp"

namespace BuggyDraw
{
    struct EdgeEndpoint : public JSON::Stringifiable <EdgeEndpoint>
                        , public JSON::Parsable <EdgeEndpoint>
    {
        boost::optional <std::string> type;
        boost::optional <std::string> node; // nodeId, naming sucks
        boost::optional <std::string> port; // portId, naming sucks
        boost::optional <std::string> kind; // in/out
    };

    struct Edge : public JSON::Stringifiable <Edge>
                , public JSON::Parsable <Edge>
    {
        EdgeEndpoint from;
        EdgeEndpoint to;
        std::string layer;
        boost::optional <bool> innerCompoundInput;
        boost::optional <bool> innerCompoundOutput;
        boost::optional <std::string> recursion;
    };
}

BOOST_FUSION_ADAPT_STRUCT
(
    BuggyDraw::EdgeEndpoint,
    (boost::optional <std::string>, type)
    (boost::optional <std::string>, node)
    (boost::optional <std::string>, port)
    (boost::optional <std::string>, kind)
)

BOOST_FUSION_ADAPT_STRUCT
(
    BuggyDraw::Edge,
    (BuggyDraw::EdgeEndpoint, from)
    (BuggyDraw::EdgeEndpoint, to)
    (std::string, layer)
    (boost::optional <bool>, innerCompoundInput)
    (boost::optional <bool>, innerCompoundOutput)
    (boost::optional <std::string>, recursion)
)
