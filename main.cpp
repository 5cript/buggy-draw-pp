#include "graph_draw.hpp"

#include <iostream>

#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/id_translator.hpp>

int main()
{
    using namespace Cairo;
    using namespace BuggyDraw;

    Graph graph;
    try
    {
        graph = loadGraphFromFile("almostFactorial.json");
        //graph = loadGraphFromFile("output_0.json");
    }
    catch (boost::property_tree::ptree_bad_path const& exc)
    {
        std::cout << exc.what() << "\n";
        std::cout << exc.path <boost::property_tree::string_path <std::string, boost::property_tree::id_translator <std::string>>> ().dump() << "\n";
    }


    GraphRenderOptions options;
    options.textPen = {5, Colors::Black};

    options.nodeMainFont.size = 16;
    options.nodeInputTypeFont.size = 14;
    options.nodeOutputTypeFont.size = 14;
    options.nodeIdFont.size = 12;

    initializeDrawInformation(graph);
    estimateSize(graph, nullptr, options);

    Cairo::Surface surface(graph.drawInformation.get().size.getWidth() + 100, graph.drawInformation.get().size.getHeight() + 100);
    Cairo::DrawContext drawContext(&surface);

    render(&drawContext, graph, options);

    surface.saveToFile("hello.png");

    return 0;
}
