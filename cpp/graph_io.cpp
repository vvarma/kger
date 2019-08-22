//
// Created by nvr on 21/8/19.
//

#include <fstream>
#include "graph_io.h"
#include "proto/graph.pb.h"

void write_named_entity(graph::Node *node, std::shared_ptr<NamedEntityNode> nen) {
    node->set_id(nen->unique_id());
    node->mutable_entity()->set_resource(nen->resource);
    node->mutable_entity()->mutable_named()->set_allocated_label(nen->label);
    for (const auto &parent:p.second->parents)
        node->mutable_entity()->mutable_named()->add_parents(parent);
}

void write_instance(graph::Node *node, std::shared_ptr<InstanceNode> in) {
    node->set_id(in->unique_id());
    node->mutable_entity()->set_resource(in->resource);
    for (const auto &p : in->relations) {
        auto r = node->mutable_entity()->mutable_instance()->add_relations();
        r->set_edgeid(p.first);
        for (const auto &parent:p.second) {
            r->add_instances(p);
        }
    }
}

void write_edge(graph::Edge *edge, std::shared_ptr<Edge> e) {
    edge->set_resource(e->resource);
    edge->set_id(e->unique_id());
}

void write_lexicals(graph::Node *node, std::shared_ptr<LexicalNode> ln) {
    node->set_id(ln->unique_id());
    for (const auto &t:ln->tokens) {
        node->mutable_lexical()->add_tokens(t);
    }
}

void save(std::shared_ptr<Graph> graph, const GraphSaveOptions &options) {
    graph::Graph g;
    for (const auto &e:graph->edges) {
        write_edge(g.add_edges(), e.second);
    }
    for (const auto &p:graph->named_entities) {
        write_named_entity(g.add_nodes(), p.second);
    }
    for (const auto &p:graph->instances) {
        write_instance(g.add_nodes(), p);
    }
    for (const auto &p:graph->lexicals) {
        write_lexicals(g.add_nodes(), p.second);
    }
    {
        std::ofstream f(options.output_path, std::ios::out | std::ios::binary | std::ios::trunc);
        g.SerializeToOstream(&f);
    }
    google::protobuf::ShutdownProtobufLibrary();
}
