//
// Created by nvr on 28/9/19.
//

#include "gtest/gtest.h"
#include "hex/hex.h"
#include "hex_utils.h"

TEST(simple_happy, HexGraph) {
    {
        HexGraph<std::string> graph(10);
        graph.add_child("a", "b");
        graph.add_child("b", "c");
        graph.add_child("a", "c");
        ASSERT_TRUE(graph.validate());
    }
    {
        HexGraph<std::string> graph(10);
        graph.add_child("d", "e");
        graph.add_child("a", "b");
        graph.add_child("b", "c");
        graph.add_child("a", "c");
        graph.add_child("b", "d");
        graph.add_child("c", "e");
        ASSERT_TRUE(graph.validate());
    }
}

TEST(loop, HexGraph) {
    HexGraph<std::string> graph(10);
    graph.add_child("a", "b");
    graph.add_child("b", "c");
    graph.add_child("c", "e");
    graph.add_child("c", "a");
    ASSERT_FALSE(graph.validate());
}

TEST(self_loop, HexGraph) {
    HexGraph<std::string> graph(10);
    graph.add_child("a", "b");
    graph.add_child("b", "c");
    graph.add_child("a", "c");
    graph.add_child("a", "a");
    ASSERT_FALSE(graph.validate());
}

TEST(disconnected, HexGraph) {
    HexGraph<std::string> graph(10);
    graph.add_child("d", "e");
    graph.add_child("a", "b");
    graph.add_child("b", "c");
    graph.add_child("a", "c");
    ASSERT_FALSE(graph.validate());
}

TEST(fix_disconnected, HexGraph) {
    HexGraph<std::string> graph(10);
    graph.add_child("d", "e");
    graph.add_child("a", "b");
    graph.add_child("b", "c");
    graph.add_child("a", "c");
    graph.add_parent_exclustions();
    ASSERT_TRUE(graph.validate());
}

class HexGraphForTest : public HexGraph<std::string> {
public:
    explicit HexGraphForTest(int initialCapacity) : HexGraph(initialCapacity) {}

    const HexGraphInternal &getInternal() const {
        return internal;
    }

    std::vector<int> getNodes() const {
        std::vector<int> n;
        for (auto p:nodes) {
            n.push_back(p.second);
        }
        return n;
    }

    std::string lookup(int node) const {
        return nodesReverse[node];
    }
};

TEST(sparsifyDensifyExclusions, HexGraph) {
    HexGraphForTest graph(10);
    graph.add_child("a", "b");
    graph.add_child("a", "c");
    graph.add_child("c", "d");
    graph.add_child("f", "g");
    graph.add_parent_exclustions();
    HexGraphIntermediate inter;
    sparsifyAndDensify(graph.getInternal(), inter);
    ASSERT_TRUE(inter.sparseExclusions[0].size() == 1);
    ASSERT_TRUE(inter.sparseExclusions[4].size() == 1);

    ASSERT_TRUE(inter.denseExclusions[0].size() == 2);
    ASSERT_TRUE(inter.denseExclusions[1].size() == 2);
    ASSERT_TRUE(inter.denseExclusions[2].size() == 2);
    ASSERT_TRUE(inter.denseExclusions[3].size() == 2);
    ASSERT_TRUE(inter.denseExclusions[4].size() == 4);
    ASSERT_TRUE(inter.denseExclusions[5].size() == 4);

    junctionGraph(inter);

}

TEST(sparsifyDensifyChildren, HexGraph) {
    HexGraphForTest graph(10);
    graph.add_child("a", "b");
    graph.add_child("b", "c");
    graph.add_child("c", "d");
    graph.add_child("a", "c");
    graph.add_parent_exclustions();
    HexGraphIntermediate inter;
    sparsifyAndDensify(graph.getInternal(), inter);

    ASSERT_TRUE(inter.sparseParentToChild[0].size() == 1);
    ASSERT_TRUE(inter.sparseParentToChild[1].size() == 1);
    ASSERT_TRUE(inter.sparseParentToChild[2].size() == 1);
    ASSERT_TRUE(inter.sparseParentToChild[3].empty());

    ASSERT_TRUE(inter.denseParentToChild[0].size() == 3);
    ASSERT_TRUE(inter.denseParentToChild[1].size() == 2);
    ASSERT_TRUE(inter.denseParentToChild[2].size() == 1);
    ASSERT_TRUE(inter.denseParentToChild[3].empty());
}
/*
 * a
 * |_ b <----
 *    |_ c  | <--|
 *    |_ d  | <--|
 * e        |
 * |_ f <----
 *    |_ g
 * */
TEST(listCliqueStateSpace, HexGraph) {
    HexGraphForTest graph(10);
    graph.add_child("a", "b");
    graph.add_child("b", "c");
    graph.add_child("b", "d");
    graph.add_exclusion("c", "d");
    graph.add_child("e", "f");
    graph.add_child("f", "g");
    graph.add_exclusion("f", "b");
    HexGraphIntermediate inter;
    sparsifyAndDensify(graph.getInternal(), inter);
    std::vector<std::set<int>> validStates;
    listCliqueStateSpace(inter, graph.getNodes(), validStates);
    std::cout << "use advanced equals from gmock" << std::endl;
    for (const auto &s:validStates) {
        for (auto sn:s) {
            std::cout << graph.lookup(sn) << "\t";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}