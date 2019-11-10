//
// Created by nvr on 28/9/19.
//

#ifndef KGER_HEX_NODE_VISITOR_H
#define KGER_HEX_NODE_VISITOR_H

#include "hex/hex.h"
#include "graph/node.h"


class HEXNodeVisitor : public NodeVisitor {
    std::shared_ptr<HexGraph<has_unique_id::uid >> graph;
public:
    explicit HEXNodeVisitor(std::shared_ptr<HexGraph<has_unique_id::uid >> graph);

    void visit(std::shared_ptr<NamedEntityNode> ptr) override;

    void visit(std::shared_ptr<InstanceNode> ptr) override;

    void visit(std::shared_ptr<LexicalNode> ptr) override;
};

#endif //KGER_HEX_NODE_VISITOR_H
