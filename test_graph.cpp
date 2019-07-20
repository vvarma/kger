//
// Created by nvr on 19/7/19.
//

#include "graph.h"
#include "gtest/gtest.h"

using namespace std;

TEST(NodeRegistery, basic) {
    auto nodes = {
            "http://www.w3.org/2002/07/owl#Thing",
            "http://www.w3.org/2002/07/owl#Thing",
            "http://dbpedia.org/ontology/Disease",
            "http://dbpedia.org/ontology/AdministrativeRegion",
            "http://dbpedia.org/ontology/OfficeHolder",
            "http://dbpedia.org/ontology/TimePeriod",
            "http://dbpedia.org/ontology/TimePeriod"
    };
    auto reg = make_shared<Registery>();
    for (auto n :nodes){
        reg->get_node(n);
    }
    ASSERT_EQ(reg->node_count(),5);

}

