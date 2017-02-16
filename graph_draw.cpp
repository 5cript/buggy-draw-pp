#include "graph_draw.hpp"
#include "buggy-graphtools-pp/graph-basics.hpp"

#include <set>

namespace Buggy
{
//#####################################################################################################################
    using namespace Cairo;
//#####################################################################################################################
    void initializeDrawInformation(Node& node)
    {
        forAllNodes(node, [](Node& n){
            n.drawInformation = DrawInformation{};
        });
    }
//#####################################################################################################################
    BoundingBox calculateAtomicNodeBounds(Node const& node, GraphRenderOptions const& options)
    {
        Surface dummySurface(0, 0);
        DrawContext dummyContext(&dummySurface);
        auto box = Text(&dummyContext, 0, 0, node.componentId, options.nodeMainFont).calculateBounds(options.textPen);

        std::string inputs;
        std::string outputs;
        for (auto const& port : node.ports)
        {
            if (port.kind == "input")
                inputs = inputs + port.type + "   ";
            if (port.kind == "output")
                outputs = outputs + port.type + "   ";
        }
        if (!inputs.empty())
            inputs = inputs.substr(0, inputs.length() - 3);
        if (!outputs.empty())
            outputs = outputs.substr(0, outputs.length() - 3);


        auto inTypeBox = Text(&dummyContext, 0, 0, inputs, options.nodeInputTypeFont).calculateBounds(options.inputTypesPen);
        auto outTypeBox = Text(&dummyContext, 0, 0, outputs, options.nodeOutputTypeFont).calculateBounds(options.outputTypesPen);
        auto idBox = Text(&dummyContext, 0, 0, node.id, options.nodeIdFont).calculateBounds(options.idPen);

        if (!inputs.empty())
            box.y2 += inTypeBox.getHeight() + 5;
        if (!outputs.empty())
            box.y2 += outTypeBox.getHeight() + 5;
        box.y2 += idBox.getHeight() + 5;

        double minWidth =
            std::max(std::max(std::max (box.getWidth(), inTypeBox.getWidth()),
                     outTypeBox.getWidth()), idBox.getWidth());

        box.setWidth (minWidth);

        box.x2 += options.internalNodeXPadding * 2;
        box.y2 += options.internalNodeYPadding * 2;

        return box;
    }
//---------------------------------------------------------------------------------------------------------------------
    void renderAtomic(DrawContext* ctx, Node const& node, GraphRenderOptions const& options)
    {
        Surface dummySurface(0, 0);
        DrawContext dummyContext(&dummySurface);
        auto totalBounds = calculateAtomicNodeBounds(node, options);

        std::string inputs;
        std::string outputs;
        for (auto const& port : node.ports)
        {
            if (port.kind == "input")
                inputs = inputs + port.type + "   ";
            if (port.kind == "output")
                outputs = outputs + port.type + "   ";
        }
        if (!inputs.empty())
            inputs = inputs.substr(0, inputs.length() - 3);
        if (!outputs.empty())
            outputs = outputs.substr(0, outputs.length() - 3);


        Rectangle rect(
            ctx,
            node.drawInformation.get().size.x,
            node.drawInformation.get().size.y,
            node.drawInformation.get().size.getWidth(),
            node.drawInformation.get().size.getHeight()
        );

        Text inputText(
            ctx,
            node.drawInformation.get().size.x + options.internalNodeXPadding,
            node.drawInformation.get().size.y + options.internalNodeYPadding,
            inputs,
            options.nodeInputTypeFont
        );
        auto inputTextBounds = inputText.calculateBounds(options.inputTypesPen);

        auto captionBounds = Text(&dummyContext, 0, 0, node.componentId, options.nodeMainFont).calculateBounds(options.textPen);
        Text caption(
            ctx,
            node.drawInformation.get().size.x + totalBounds.getWidth() / 2 - captionBounds.getWidth() / 2 + options.internalNodeXPadding,
            node.drawInformation.get().size.y + options.internalNodeYPadding + inputTextBounds.getHeight() + 5,
            node.componentId,
            options.nodeMainFont
        );

        Text id(
            ctx,
            node.drawInformation.get().size.x + options.internalNodeXPadding,
            node.drawInformation.get().size.y + options.internalNodeYPadding + inputTextBounds.getHeight() + 5 + captionBounds.getHeight() + 5,
            std::string{"("} + node.id + ")",
            options.nodeIdFont
        );
        //auto idBounds = id.calculateBounds(options.idPen);

        auto outputTypeBounds = Text(&dummyContext, 0, 0, node.componentId, options.nodeOutputTypeFont).calculateBounds(options.outputTypesPen);
        Text outputText(
            ctx,
            node.drawInformation.get().size.x + options.internalNodeXPadding,
            node.drawInformation.get().size.y + totalBounds.getHeight() - outputTypeBounds.getHeight(),
            outputs,
            options.nodeOutputTypeFont
        );

        rect.draw(options.nodeStrokePen, options.nodeFillPen);
        inputText.draw(options.inputTypesPen);
        caption.draw(options.textPen);
        id.draw(options.idPen);
        outputText.draw(options.outputTypesPen);
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
    void render(DrawContext* ctx, Graph const& graph, GraphRenderOptions const& options)
    {
        auto mutableGraph = graph;
        initializeDrawInformation(mutableGraph);
        estimateSize(mutableGraph, nullptr, options);

        saveGraphToFile(mutableGraph, "test.json");

        for (auto const& node : mutableGraph.nodes.get())
        {
            renderNode(ctx, node, options);
        }
    }
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
}
