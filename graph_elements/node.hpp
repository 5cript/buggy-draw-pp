#pragma once

#include "port.hpp"
#include "settings.hpp"
#include "edge.hpp"
#include "draw_information.hpp"

#include "../json_convert.hpp"

#include <functional>

namespace BuggyDraw
{
    struct Node : public JSON::Stringifiable <Node>
                , public JSON::Parsable <Node>
    {
        std::string id;
        std::vector <Port> ports;
        Settings settings;
        boost::optional <std::vector <Node>> nodes;
        boost::optional <bool> atomic;
        std::string componentId;
        boost::optional <std::string> version;
        boost::optional <std::vector <std::string>> path;
        std::vector <Edge> edges;
        boost::optional <DrawInformation> drawInformation; // not fundamental part of the graph, but simplifies graph drawing by a lot. (dirty, I know)
    };

    bool isAtomic(Node const& node); // aka is compound
    bool containsCompound(Node const& node);
    Node* getNodeById(Node* node, std::string const& id);
    int countInputs(Node const& node);
    void forAllNodes(Node& baseNode, std::function <void(Node&)> const& modifier);
    void initializeDrawInformation(Node& node);
}

BOOST_FUSION_ADAPT_STRUCT
(
    BuggyDraw::Node,
    (std::string, id)
    (std::vector <BuggyDraw::Port>, ports)
    (BuggyDraw::Settings, settings)
    (boost::optional <std::vector <BuggyDraw::Node>>, nodes)
    (boost::optional <bool>, atomic)
    (std::string, componentId)
    (boost::optional <std::string>, version)
    (boost::optional <std::vector <std::string> >, path)
    (std::vector <BuggyDraw::Edge>, edges)
    (boost::optional <BuggyDraw::DrawInformation>, drawInformation)
)
