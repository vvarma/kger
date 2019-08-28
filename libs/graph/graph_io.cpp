//
// Created by nvr on 21/8/19.
//

#include <fstream>
#include "graph/graph_io.h"
#include "graph_io.pb.h"

void write_named_entity(graph::Node *node, const std::shared_ptr<NamedEntityNode> &nen) {
    node->set_id(nen->unique_id());
    node->mutable_entity()->set_resource(nen->resource);
    node->mutable_entity()->mutable_named()->set_label(nen->label);
    for (const auto &parent:nen->parents)
        node->mutable_entity()->mutable_named()->add_parents(parent);
}

void write_instance(graph::Node *node, const std::shared_ptr<InstanceNode> &in) {
    node->set_id(in->unique_id());
    node->mutable_entity()->set_resource(in->resource);
    for (const auto &p : in->relations) {
        auto r = node->mutable_entity()->mutable_instance()->add_relations();
        r->set_edgeid(p.first);
        for (const auto &parent:p.second) {
            r->add_instances(parent);
        }
    }
}

void write_edge(graph::Edge *edge, const std::shared_ptr<Edge> &e) {
    edge->set_resource(e->resource);
    edge->set_id(e->unique_id());
}

void write_lexicals(graph::Node *node, const std::shared_ptr<LexicalNode> &ln) {
    node->set_id(ln->unique_id());
    for (const auto &t:ln->tokens) {
        node->mutable_lexical()->add_tokens(t);
    }
}

void save(const std::shared_ptr<Graph> &graph, const GraphSaveOptions &options) {
    graph::Graph g;
    for (const auto &e:graph->edges) {
        write_edge(g.add_edges(), e.second);
    }
    for (const auto &p:graph->named_entities) {
        write_named_entity(g.add_nodes(), p.second);
    }
    for (const auto &p:graph->instances) {
        write_instance(g.add_nodes(), p.second);
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

std::shared_ptr<Graph> load(const GraphSaveOptions &options) {
    std::shared_ptr<Graph> lg;
    graph::Graph g;
    {
        std::ifstream f(options.output_path, std::ios::in | std::ios::binary);
        if (!f.is_open()) {
            std::cerr << "Failed to open file at " << options.output_path << std::endl;
            return lg;
        }
        if (!g.ParseFromIstream(&f)) {
            std::cerr << "Failed to parse proto object from file " << options.output_path << std::endl;
            return lg;
        }
    }

    lg = std::make_shared<Graph>();
    for (const auto &e: g.edges()) {
        lg->edges[e.id()] = std::make_shared<Edge>(e.id(), e.resource());
    }
    for (const auto &n:g.nodes()) {
        if (n.has_lexical()) {
            std::vector<int> tokens;
            std::copy(n.lexical().tokens().begin(), n.lexical().tokens().end(), std::back_inserter(tokens));
            lg->lexicals[n.id()] = std::make_shared<LexicalNode>(n.id(), tokens);
        } else if (n.has_entity()) {
            if (n.entity().has_named()) {
                std::vector<has_unique_id::uid> parents;
                std::copy(n.entity().named().parents().begin(), n.entity().named().parents().end(),
                          std::back_inserter(parents));
                lg->named_entities[n.id()] = std::make_shared<NamedEntityNode>(n.id(), n.entity().resource(), parents,
                                                                               n.entity().named().label());
            } else if (n.entity().has_instance()) {
                std::map<has_unique_id::uid, std::set<has_unique_id::uid>> relations;
                for (const auto &r:n.entity().instance().relations()) {
                    std::set<has_unique_id::uid> instances;
                    std::copy(r.instances().begin(), r.instances().end(), std::inserter(instances, instances.begin()));
                    relations[r.edgeid()] = instances;
                }
                lg->instances[n.id()] = std::make_shared<InstanceNode>(n.id(), n.entity().resource(), relations);
            }
        }
    }
    return lg;

}
