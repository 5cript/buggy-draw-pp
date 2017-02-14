#include "graph.hpp"

#include "../json_convert.hpp"

#include <fstream>

namespace BuggyDraw
{
//#####################################################################################################################
    Graph loadGraphFromFile(std::string const& fileName)
    {
        std::ifstream reader(fileName, std::ios_base::binary);
        reader.seekg(0, std::ios_base::end);
        std::string buffer(reader.tellg(), ' ');
        reader.seekg(0);
        reader.read(&buffer[0], buffer.size());

        using namespace std::literals::string_literals;

        buffer = "{\"graph\":"s + buffer + "}";

        return parse <Graph> (buffer, "graph");
    }
//---------------------------------------------------------------------------------------------------------------------
    void saveGraphToFile(Graph const& graph, std::string const& fileName)
    {
        std::ofstream writer(fileName, std::ios_base::binary);
        saveGraphToStream(writer, graph);
    }
//---------------------------------------------------------------------------------------------------------------------
    std::ostream& saveGraphToStream(std::ostream& stream, Graph const& graph)
    {
        stringify(stream, graph, "");
        return stream;
    }
//#####################################################################################################################
}
