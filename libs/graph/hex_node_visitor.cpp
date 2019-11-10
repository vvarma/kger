#include <utility>

//
// Created by nvr on 28/9/19.
//

#include "hex_node_visitor.h"

void HEXNodeVisitor::visit(std::shared_ptr<NamedEntityNode> ptr) {
    for (auto &p:ptr->parents) {
        graph->add_child(p, ptr->unique_id());
    }
}

void HEXNodeVisitor::visit(std::shared_ptr<InstanceNode> ptr) {

}

void HEXNodeVisitor::visit(std::shared_ptr<LexicalNode> ptr) {

}

HEXNodeVisitor::HEXNodeVisitor(std::shared_ptr<HexGraph<has_unique_id::uid >> graph) : graph(std::move(graph)) {
}
