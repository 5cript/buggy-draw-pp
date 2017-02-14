#pragma once

#include "graph_elements/graph.hpp"
#include "cairo-wrap/cairo_wrap.hpp"

namespace BuggyDraw
{
    struct GraphRenderOptions
    {
        double minXNodePadding = 50.;
        double minYNodePadding = 20.;
        double internalNodeXPadding = 10.;
        double internalNodeYPadding = 10.;
        double compoundMarginX = 5.;
        double compoundMarginY = 5.;

        Cairo::Font nodeMainFont;
        Cairo::Pen textPen;
        Cairo::Pen nodeFillPen = Cairo::Colors::White;
        Cairo::Pen nodeStrokePen = {5, Cairo::Colors::Black};
    };

    struct GraphRenderContext
    {

    };

    void render(Cairo::DrawContext* ctx, Graph const& graph, GraphRenderOptions const& options);
}
