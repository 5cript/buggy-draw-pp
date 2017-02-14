#include "graph_draw.hpp"

#include <set>

namespace BuggyDraw
{
    using namespace Cairo;
//#####################################################################################################################
    void renderAtomic(DrawContext* ctx, Node const& node, GraphRenderOptions const& options)
    {
        Rectangle rect(
            ctx,
            node.drawInformation.get().size.x,
            node.drawInformation.get().size.y,
            node.drawInformation.get().size.getWidth(),
            node.drawInformation.get().size.getHeight()
        );

        Text caption(
            ctx,
            node.drawInformation.get().size.x + options.internalNodeXPadding,
            node.drawInformation.get().size.y + options.internalNodeYPadding,
            node.componentId,
            options.nodeMainFont
        );

        rect.draw(options.nodeStrokePen, options.nodeFillPen);
        caption.draw(options.textPen);
    }
//---------------------------------------------------------------------------------------------------------------------
    void renderNode(DrawContext* ctx, Node const& node, GraphRenderOptions const& options)
    {
        if (isAtomic(node))
        {
            renderAtomic(ctx, node, options);
        }
        else if (node.nodes)
        {
            for (auto const& node: node.nodes.get())
            {
                renderNode(ctx, node, options);
            }
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    BoundingBox calculateAtomicNodeBounds(Node const& node, GraphRenderOptions const& options)
    {
        Surface dummySurface(0, 0);
        DrawContext dummyContext(&dummySurface);
        auto box = Text(&dummyContext, 0, 0, node.componentId, options.nodeMainFont).calculateBounds(options.textPen);
        box.x2 += options.internalNodeXPadding * 2;
        box.y2 += options.internalNodeYPadding * 2;

        return box;
    }
//---------------------------------------------------------------------------------------------------------------------
    void estimateSize(Graph& graph, Graph* parent, GraphRenderOptions const& options)
    {
        if (!parent)
            graph.drawInformation.get().size.move(0, 0);
        else
            graph.drawInformation.get().size.move(parent->drawInformation.get().size.x + options.compoundMarginX,
                                                  parent->drawInformation.get().size.y + options.compoundMarginY);

        if (isAtomic(graph))
        {
            // is atomic
            auto box = calculateAtomicNodeBounds(graph, options);
            graph.drawInformation.get().size.setWidth(box.getWidth());
            graph.drawInformation.get().size.setHeight(box.getHeight());
            return;
        }
        else
        {
            // is compound
            std::vector <std::set <Node*>> nodeLayers;

            auto insertNodeToLayer = [&](Node* node, int layer)
            {
                // expand layer container to needed size
                for (int i = nodeLayers.size() - 1; i < layer; ++i)
                    nodeLayers.emplace_back();

                node->drawInformation.get().nodeLevel = layer;

                // there must not be duplicates.
                int layerCounter = 0;
                for (auto& nodeLayer : nodeLayers)
                {
                    for (auto iter = std::begin(nodeLayer); iter != std::end(nodeLayer); ++iter)
                    {
                        if ((*iter)->id == node->id && layerCounter < layer)
                        {
                            nodeLayer.erase(iter);
                            break;
                        }
                    }
                    ++layerCounter;
                }

                std::cout << node->componentId << "(" << layer << ")\n";

                nodeLayers[layer].insert(node);
            };

            // determine node level.
            for (bool noMoreUnsolvedNodes; !noMoreUnsolvedNodes;) // FIXME: better algorithm is possible. Buggy.Rewrite would be useful
            {
                noMoreUnsolvedNodes = true;
                for (auto& edge: graph.edges)
                {
                    if (edge.layer != "dataflow")
                        continue;

                    if (edge.from.node == graph.id)
                    {
                        auto* node = getNodeById(&graph, edge.to.node.get());
                        if (node == nullptr)
                            throw std::runtime_error("edge.to does not connect to any valid node.");
                        else
                            insertNodeToLayer(node, 0);
                    }
                    else if (edge.to.node == graph.id)
                    {
                        continue; // already done.
                    }
                    else
                    {
                        auto nodeFromId = [&graph](EdgeEndpoint endpoint)
                        {
                            auto* node = getNodeById(&graph, endpoint.node.get());
                            if (node == nullptr)
                                throw std::runtime_error("edge does not connect to any valid node.");
                            return node;
                        };

                        if (edge.from.node && !edge.to.node)
                        {
                            throw std::runtime_error("Edge does not go into anything");
                        }
                        else if (!edge.from.node && edge.to.node)
                        {
                            insertNodeToLayer(nodeFromId(edge.to), 0);
                            continue;
                        }
                        else if (!edge.from.node && !edge.to.node)
                        {
                            throw std::runtime_error("open edge with no nodes found");
                        }


                        if (!edge.from.port || !edge.to.port)
                        {
                            // throw std::runtime_error("what does this even mean 2?");
                        }

                        auto* nodeFrom = nodeFromId(edge.from);
                        auto* nodeTo = nodeFromId(edge.to);

                        if (!nodeFrom->drawInformation.get().nodeLevel)
                        {
                            // some nodes do not have inputs:
                            if (countInputs(*nodeFrom) == 0)
                            {
                                insertNodeToLayer(nodeFrom, 0);
                                insertNodeToLayer(nodeTo, 1);
                            }
                            else
                                noMoreUnsolvedNodes = false;
                        }
                        else
                        {
                            insertNodeToLayer(nodeTo, nodeFrom->drawInformation.get().nodeLevel.get() + 1);
                        }
                    }
                }
            }

            // go calculate padding by layer
            double previousHeight = options.minYNodePadding;
            double maxLayerWidth = 0.;
            for (auto const& layer : nodeLayers)
            {
                graph.drawInformation.get().yLayerStart = previousHeight;
                for (auto* node : layer)
                    estimateSize(*node, &graph, options);

                double maxHeight = 0.;
                double maxWidth = 0.;
                for (auto* node : layer) // for all nodes in the current layer
                {
                    maxHeight = std::max(maxHeight, node->drawInformation.get().size.getHeight());
                    maxWidth = std::max(maxWidth, node->drawInformation.get().size.getWidth());
                }

                int nodeCounter = 0;
                for (auto* node : layer) // set position of every node in this compound.layer
                {
                    node->drawInformation.get().size.additiveMove(
                        nodeCounter * maxWidth + options.minXNodePadding * (nodeCounter + 1) + (parent?parent->drawInformation.get().xLayerStart:0),
                        previousHeight + options.minYNodePadding + (parent?parent->drawInformation.get().yLayerStart:0)
                    );
                    nodeCounter++;
                }

                maxLayerWidth = std::max(maxLayerWidth, nodeCounter * maxWidth + options.minXNodePadding * (nodeCounter + 1));
                previousHeight += maxHeight + options.minYNodePadding;
            }
            graph.drawInformation.get().size.setWidth(maxLayerWidth);
            graph.drawInformation.get().size.setHeight(previousHeight);
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    void finalizeSizeEstimation(Graph& current, Graph& parent, GraphRenderOptions const& options)
    {
        double xOffset = parent.drawInformation.get().size.x + options.compoundMarginX;
        double yOffset = parent.drawInformation.get().size.y + options.compoundMarginY;

        //if (isAtomic(current))
        //  current.drawInformation.get().size.move(xOffset, yOffset);

        if (!current.nodes)
            return;
        for (auto& node : current.nodes.get())
        {
            finalizeSizeEstimation(node, current, options);
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    void finalizeSizeEstimation(Graph& root, GraphRenderOptions const& options)
    {
        if (!root.nodes)
            return;
        for (auto& node : root.nodes.get())
        {
            finalizeSizeEstimation(node, root, options);
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    void render(DrawContext* ctx, Graph const& graph, GraphRenderOptions const& options)
    {
        auto mutableGraph = graph;
        initializeDrawInformation(mutableGraph);
        estimateSize(mutableGraph, nullptr, options);
        //finalizeSizeEstimation(mutableGraph, options);

        saveGraphToFile(mutableGraph, "test.json");

        for (auto const& node : mutableGraph.nodes.get())
        {
            renderNode(ctx, node, options);
        }
    }
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
}
