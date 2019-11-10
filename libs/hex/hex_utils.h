//
// Created by nvr on 2/10/19.
//

#ifndef KGER_HEX_UTILS_H
#define KGER_HEX_UTILS_H

struct HexGraphIntermediate {
    int numVertices = 0;
    std::vector<std::set<int>> sparseParentToChild;
    std::vector<std::set<int>> denseParentToChild;
    std::vector<std::set<int>> denseChildToParent;
    std::vector<std::set<int>> sparseChildToParent;
    std::vector<std::set<int>> sparseExclusions;
    std::vector<std::set<int>> denseExclusions;

    std::vector<std::vector<int>> cliques;
    std::map<int, std::vector<int>> nodeToClique;

    std::vector<int> upPass;
    std::vector<int> cliqueParents;
    std::vector<std::vector<int>> cliqueChildren;

    std::vector<std::vector<std::set<int>>> states;

    std::vector<std::map<std::pair<int, int>, std::vector<int>>> supProduct;
};

void sparsifyAndDensify(const HexGraphInternal &hex, HexGraphIntermediate &inter);

void junctionGraph(HexGraphIntermediate &hex);

void junctionTree(HexGraphIntermediate &hex);

void listCliqueStateSpace(HexGraphIntermediate &hex, std::vector<int> clique, std::vector<std::set<int>> &states,
                          std::set<int> curState = {});

void listStateSpace(HexGraphIntermediate &hex);

void recordSumProduct(HexGraphIntermediate &hex);

#endif //KGER_HEX_UTILS_H
