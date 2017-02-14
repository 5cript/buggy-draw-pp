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
    }
    catch (boost::property_tree::ptree_bad_path const& exc)
    {
        std::cout << exc.what() << "\n";
        std::cout << exc.path <boost::property_tree::string_path <std::string, boost::property_tree::id_translator <std::string>>> ().dump() << "\n";
    }

    Cairo::Surface surface(800, 800);
    Cairo::DrawContext drawContext(&surface);

    GraphRenderOptions options;
    options.textPen = {5, Colors::Black};

    render(&drawContext, graph, options);

    surface.saveToFile("hello.png");

    return 0;
}
