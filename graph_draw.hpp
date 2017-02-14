#pragma once

#include "graph_elements/graph.hpp"
#include "cairo-wrap/cairo_wrap.hpp"

namespace BuggyDraw
{
    struct GraphRenderOptions
    {
        double minXNodePadding = 50.;
        double minYNodePadding = 20.;
        double internalNodeXPadding = 8.;
        double internalNodeYPadding = 8.;
        double compoundMarginX = 5.;
        double compoundMarginY = 5.;

        Cairo::Font nodeMainFont;
        Cairo::Font nodeInputTypeFont;
        Cairo::Font nodeOutputTypeFont;
        Cairo::Font nodeIdFont;
        Cairo::Pen textPen = Cairo::Colors::Black;
        Cairo::Pen inputTypesPen = Cairo::Colors::Red;
        Cairo::Pen outputTypesPen = Cairo::Colors::Red;
        Cairo::Pen idPen = Cairo::Colors::Blue;
        Cairo::Pen nodeFillPen = Cairo::Colors::White;
        Cairo::Pen nodeStrokePen = {5, Cairo::Colors::Black};
    };

    struct GraphRenderContext
    {

    };

    void estimateSize(Graph& graph, Graph* parent, GraphRenderOptions const& options);
    void render(Cairo::DrawContext* ctx, Graph const& graph, GraphRenderOptions const& options);
}
