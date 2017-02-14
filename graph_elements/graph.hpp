#pragma once

#include "node.hpp"

#include <iostream>

namespace BuggyDraw
{
    using Graph = Node; // Graph and Node are interchangeable.

    Graph loadGraphFromFile(std::string const& fileName);
    void saveGraphToFile(Graph const& graph, std::string const& fileName);
    std::ostream& saveGraphToStream(std::ostream& stream, Graph const& graph);
}
