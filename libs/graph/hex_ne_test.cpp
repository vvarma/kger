//
// Created by nvr on 28/9/19.
//

#include "hex_node_visitor.h"
#include "gtest/gtest.h"
#include "owlparser.h"
#include "glog/logging.h"

std::string file_name;

TEST(ingest_owl, OWLToHEX) {
    OntologyBuilder owl;
    owl.processTree(file_name);
    auto hex_graph = std::make_shared<HexGraph<has_unique_id::uid >>(10);
    HEXNodeVisitor visitor(hex_graph);
    for (const auto &p:owl.nodes) {
        p.second->accept(&visitor);
    }

    auto exclusions = hex_graph->add_parent_exclustions();
    LOG(INFO) << "Printing exclusions" << std::endl;
    for (const auto &ex:exclusions) {
        for (auto exNo:ex.second) {
            LOG(INFO) << owl.nodes[ex.first]->label << " --- " << owl.nodes[exNo]->label;
        }
    }
    ASSERT_TRUE(hex_graph->validate());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    google::InitGoogleLogging(argv[0]);
    file_name = argv[1];
    return RUN_ALL_TESTS();
}
