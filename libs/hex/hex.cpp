//
// Created by nvr on 23/9/19.
//

#include "hex/hex.h"
#include "hex_utils.h"
#include <algorithm>
#include <queue>
#include <tuple>

void HexGraphInternal::resize(int maxNodes) {
    if (maxNodes > capacity) {
        parentToChild.resize(maxNodes);
        childToParent.resize(maxNodes);
        exclusions.resize(maxNodes);
        capacity = maxNodes;
    }
}

void HexGraphInternal::add_child(int parent, int child) {
    if (parent >= height || child >= height) {
        if (parent >= capacity || child >= capacity) {
            resize(capacity * 2);
        }
        height = (parent > child ? parent : child) + 1;
    }
    parentToChild[parent].insert(child);
    childToParent[child].insert(parent);
}

void HexGraphInternal::add_exclusion(int node1, int node2) {
    if (node1 >= height || node2 >= height) {
        if (node1 >= capacity || node2 >= capacity) {
            return; // status
        }
        height = (node1 > node2 ? node1 : node2) + 1;
    }
    exclusions[node1].insert(node2);
    exclusions[node2].insert(node1);
}

HexGraphInternal::HexGraphInternal(int capacity) : parentToChild(capacity), childToParent(capacity),
                                                   exclusions(capacity), capacity(capacity),
                                                   height(0) {
}

std::map<int, std::set<int>> HexGraphInternal::add_parent_exclusions() {
    std::vector<std::set<int >> disconnectedParents;
    {
        std::set<int> to_visit;
        for (int i = 0; i < height; ++i) {
            to_visit.insert(i);
        }
        std::queue<int> visit_queue;
        while (!to_visit.empty()) {
            std::set<int> parentsWrelatedChildren;
            visit_queue.push(*to_visit.begin());
            while (!visit_queue.empty()) {
                auto visiting = visit_queue.front();
                visit_queue.pop();
                if (to_visit.find(visiting) == to_visit.end())continue;
                if (childToParent[visiting].empty()) {
                    parentsWrelatedChildren.insert(visiting);
                }
                for (auto n:parentToChild[visiting])visit_queue.push(n);
                for (auto n:exclusions[visiting])visit_queue.push(n);
                for (auto n:childToParent[visiting])visit_queue.push(n);
                to_visit.erase(visiting);
            }
            disconnectedParents.push_back(parentsWrelatedChildren);
        }
    }
    std::map<int, std::set<int>> parentExclusions;
    for (int j = 0; j < disconnectedParents.size(); ++j) {
        for (int i = j + 1; i < disconnectedParents.size(); ++i) {
            for (auto node:disconnectedParents[j]) {
                for (auto otherNode:disconnectedParents[i]) {
                    add_exclusion(node, otherNode);
                    parentExclusions[node].insert(otherNode);
                }
            }
        }
    }
    return parentExclusions;
}


bool isCyclic(const std::vector<std::set<int>> &adjs, int current_node, std::vector<bool> &visited,
              std::vector<bool> &rec_stack) {
    if (!visited[current_node]) {
        visited[current_node] = true;
        rec_stack[current_node] = true;
        for (auto p:adjs[current_node]) {
            if (!visited[p] && isCyclic(adjs, p, visited, rec_stack)) {
                return true;
            }
            if (rec_stack[p])
                return true;
        }
    }
    rec_stack[current_node] = false;
    return false;
}

bool HexGraphInternal::validate() {
    for (int i = 0; i < height; ++i) {
        if (!exclusions[i].empty() && exclusions[i].find(i) != exclusions[i].end()) {
            // found self loop in exclusions
            return false;
        }
        if (!parentToChild[i].empty() && parentToChild[i].find(i) != parentToChild[i].end()) {
            // found self loop in hierarchial
            return false;
        }
        std::vector<int> intersect(parentToChild[i].size() + exclusions[i].size());
        auto it = std::set_intersection(parentToChild[i].begin(), parentToChild[i].end(), exclusions[i].begin(),
                                        exclusions[i].end(),
                                        intersect.begin());
        intersect.resize(it - intersect.begin());
        if (!intersect.empty()) {
            // hierarchy and exclusion cannot appear together
            return false;
        }
    }
    std::set<int> to_visit;
    for (int i = 0; i < height; ++i) {
        to_visit.insert(i);
    }
    std::queue<int> visit_queue;
    visit_queue.push(0);
    while (!visit_queue.empty()) {
        auto visiting = visit_queue.front();
        visit_queue.pop();
        if (to_visit.find(visiting) == to_visit.end())continue;
        for (auto n:parentToChild[visiting])visit_queue.push(n);
        for (auto n:exclusions[visiting])visit_queue.push(n);
        for (int i = 0; i < height; ++i) {
            if (parentToChild[i].find(visiting) != parentToChild[i].end())visit_queue.push(i);
        }
        to_visit.erase(visiting);
    }
    if (!to_visit.empty()) {
        // not fully connected
        return false;
    }
    std::vector<bool> visited(height);
    std::vector<bool> rec_stack(height);
    for (int j = 0; j < height; ++j) {
        if (isCyclic(parentToChild, j, visited, rec_stack))
            // found loop in parentToChild
            return false;
    }


    // check if fully connected either via hierarchial or exclusion
    // check for constraints acyclic and dag
    // check for exclusion betweeen node and ancestors
    {
        for (int i = 0; i < height; ++i) {
            std::set<int> ancestors;
            std::queue<int> ancestror_probe;
            ancestror_probe.push(i);
            ancestors.insert(i);
            while (!ancestror_probe.empty()) {
                auto ancestor = ancestror_probe.front();
                ancestror_probe.pop();
                for (int j = 0; j < height; ++j) {
                    if (parentToChild[j].find(ancestor) != parentToChild[j].end()) {
                        ancestors.insert(j);
                        ancestror_probe.push(j);
                    }
                }
            }
            while (!ancestors.empty()) {
                auto it = ancestors.begin();
                auto element = *it;
                it = ancestors.erase(it);
                std::vector<int> intersect(exclusions[element].size() + ancestors.size());
                auto intersect_it = std::set_intersection(exclusions[element].begin(), exclusions[element].end(), it,
                                                          ancestors.end(),
                                                          intersect.begin());
                intersect.resize(intersect_it - intersect.begin());
                if (!intersect.empty()) {
                    // exclusion between one of the ancestors of this node i
                    return false;
                }
            }
        }
    }

    return true;
}

int HexGraphInternal::size() const {
    return height;
}

// Util functions on internal structure
void junctionGraph(HexGraphIntermediate &hex) {
    // create undirected graph for variable elimination
    auto elimEdges = hex.sparseExclusions;
    for (int i = 0; i < hex.sparseParentToChild.size(); ++i) {
        for (auto n:hex.sparseParentToChild[i]) {
            elimEdges[i].insert(n);
            elimEdges[n].insert(i);
        }
    }
    // minimal fill
    std::vector<int> eliminationOrder;
    {
        auto elimTemp = elimEdges;
        int k = 0;
        std::vector<bool> visited(hex.numVertices);
        while (k < hex.numVertices) {
            int minNeighbors = hex.numVertices + 1;
            std::vector<int> nextMinFill;
            for (int i = 0; i < hex.numVertices; ++i) {
                if (visited[i])continue;
                if (elimTemp[i].size() < minNeighbors) {
                    nextMinFill.clear();
                    nextMinFill.push_back(i);
                    minNeighbors = elimTemp[i].size();
                } else if (elimTemp[i].size() == minNeighbors) {
                    nextMinFill.push_back(i);
                } else {
                    continue;
                }
            }
            std::copy(nextMinFill.begin(), nextMinFill.end(), std::back_inserter(eliminationOrder));
            for (auto eV:nextMinFill) {
                visited[eV] = true;
                for (auto nei:elimTemp[eV]) {
                    elimTemp[nei].erase(eV);
                }
                elimTemp[eV].clear();
            }
            k += nextMinFill.size();
        }
    }
    std::vector<std::vector<int>> cliques;
    int jtWidth = 0;
    {
        for (auto eV:eliminationOrder) {
            std::vector<int> clique;
            std::copy(elimEdges[eV].begin(), elimEdges[eV].end(), std::back_inserter(clique));
            if (elimEdges[eV].size() > jtWidth) jtWidth = elimEdges[eV].size();
            if (elimEdges[eV].empty())continue;
            for (int i = 0; i < clique.size() - 1; ++i) {
                for (int j = i + 1; j < clique.size(); ++j) {
                    elimEdges[i].insert(j);
                    elimEdges[j].insert(i);
                }
            }
            for (auto nei:clique) {
                elimEdges[nei].erase(eV);
            }
            elimEdges[eV].clear();
            clique.push_back(eV);
            cliques.push_back(clique);
        }
    }
    // remove duplicate cliques
    {
        std::set<int> duplicateClique;
        for (int l = 0; l < cliques.size() - 1; ++l) {
            for (int i = l + 1; i < cliques.size(); ++i) {
                if (duplicateClique.find(i) != duplicateClique.end() ||
                    duplicateClique.find(l) != duplicateClique.end()) {
                    continue;
                }
                std::vector<int> intersect;
                std::set_intersection(cliques[l].begin(), cliques[l].end(), cliques[i].begin(), cliques[i].end(),
                                      intersect.begin());
                if (intersect.size() == cliques[l].size())duplicateClique.insert(l);
                else if (intersect.size() == cliques[i].size())duplicateClique.insert(i);
            }
        }
        for (auto dup:duplicateClique) {
            cliques[dup] = cliques.back();
            cliques.pop_back();
        }
    }
    hex.cliques = cliques;
    for (int i = 0; i < cliques.size(); ++i) {
        auto clique = cliques[i];
        for (auto c:clique) {
            hex.nodeToClique[c].push_back(i);
        }
    }

}

void sparsifyAndDensify(const HexGraphInternal &hex, HexGraphIntermediate &inter) {
    inter.numVertices = hex.size();
    {
        inter.sparseParentToChild = hex.parentToChild;
        inter.denseParentToChild = hex.parentToChild;
        for (int i = 0; i < hex.height; ++i) {
            std::queue<int> temp;
            std::queue<int> descendents; // except direct children
            for (auto child:hex.parentToChild[i]) {
                temp.push(child);
            }
            while (!temp.empty()) {
                auto child = temp.front();
                temp.pop();
                for (auto descendant:hex.parentToChild[child]) {
                    temp.push(descendant);
                    descendents.push(descendant);
                }
            }
            while (!descendents.empty()) {
                auto descendant = descendents.front();
                descendents.pop();
                if (hex.parentToChild[i].find(descendant) != hex.parentToChild[i].end()) {
                    inter.sparseParentToChild[i].erase(descendant);
                } else {
                    inter.denseParentToChild[i].insert(descendant);
                }
            }
        }
    }
    {
        inter.sparseChildToParent = hex.childToParent;
        inter.denseChildToParent = hex.childToParent;
        for (int i = 0; i < hex.height; ++i) {
            std::queue<int> temp;
            std::queue<int> parents; // except direct children
            for (auto parent:hex.childToParent[i]) {
                temp.push(parent);
            }
            while (!temp.empty()) {
                auto parent = temp.front();
                temp.pop();
                for (auto p :hex.childToParent[parent]) {
                    temp.push(p);
                    parents.push(p);
                }
            }
            while (!parents.empty()) {
                auto parent = parents.front();
                parents.pop();
                if (hex.childToParent[i].find(parent) != hex.childToParent[i].end()) {
                    inter.sparseChildToParent[i].erase(parent);
                } else {
                    inter.denseChildToParent[i].insert(parent);
                }
            }
        }
    }
    {
        inter.sparseExclusions = hex.exclusions;
        inter.denseExclusions = hex.exclusions;
        for (int i = 0; i < hex.height - 1; ++i) {
            for (int j = i + 1; j < hex.height; ++j) {

                // i is j's child or vice versa
                if (hex.parentToChild[i].find(j) != hex.parentToChild[i].end() ||
                    hex.parentToChild[j].find(i) != hex.parentToChild[j].end())
                    continue;
                std::queue<int> tempI;
                std::vector<int> ancestorI;
                std::queue<int> tempJ;
                std::vector<int> ancestorJ;
                tempI.push(i);
                ancestorI.push_back(i);
                while (!tempI.empty()) {
                    auto node = tempI.front();
                    tempI.pop();
                    for (auto parent:hex.childToParent[node]) {
                        tempI.push(parent);
                        ancestorI.push_back(parent);
                    }
                }
                tempJ.push(j);
                ancestorJ.push_back(j);
                while (!tempJ.empty()) {
                    auto node = tempJ.front();
                    tempJ.pop();
                    for (auto parent:hex.childToParent[node]) {
                        tempJ.push(parent);
                        ancestorJ.push_back(parent);
                    }
                }
                auto found = false;
                for (auto a1:ancestorI) {
                    for (auto a2:ancestorJ) {
                        if (hex.exclusions[a1].find(a2) != hex.exclusions[a1].end()) {
                            if (!(a1 == i && a2 == j) && !(a2 == i && a1 == j)) {
                                found = true;
                                break;
                            }
                        }
                    }
                    if (found)break;
                }
                if (found) {
                    if (hex.exclusions[i].find(j) != hex.exclusions[i].end()) {
                        inter.sparseExclusions[i].erase(j);
                        inter.sparseExclusions[j].erase(i);
                    } else {
                        inter.denseExclusions[i].insert(j);
                        inter.denseExclusions[j].insert(i);
                    }
                }
            }
        }
    }
}

void junctionTree(HexGraphIntermediate &hex) {
    auto numCliques = hex.cliques.size();
    std::vector<std::tuple<int, int, int>> cliqueEdges;
    {
        for (int i = 0; i < numCliques - 1; ++i) {
            for (int j = i + 1; j < numCliques; ++j) {
                auto clique1 = hex.cliques[i];
                auto clique2 = hex.cliques[j];
                std::vector<int> intersect;
                std::set_intersection(clique1.begin(), clique1.end(), clique2.begin(), clique2.end(),
                                      intersect.begin());
                if (!intersect.empty()) {
                    cliqueEdges.emplace_back(i, j, intersect.size());
                }
            }
        }
    }
    // kruskal - max spanning tree
    std::vector<std::vector<int>> spanningAdj(numCliques);
    {
        //sort descending by weight
        std::sort(cliqueEdges.begin(), cliqueEdges.end(),
                  [](const std::tuple<int, int, int> c1, const std::tuple<int, int, int> c2) -> bool {
                      return std::get<2>(c1) < std::get<2>(c2);
                  });
        std::vector<int> cycleCheck(numCliques);
        int spanningTreeWeight = 0;
        for (int i = 0; i < cliqueEdges.size(); ++i) {
            auto from = std::get<0>(cliqueEdges[i]);
            auto to = std::get<1>(cliqueEdges[i]);
            auto weight = std::get<2>(cliqueEdges[i]);

            if (cycleCheck[from] == 0 && cycleCheck[to] == 0) {
                cycleCheck[from] = i;
                cycleCheck[to] = i;
            } else if (cycleCheck[from] == 0) {
                cycleCheck[from] = cycleCheck[to];
            } else if (cycleCheck[to] == 0) {
                cycleCheck[to] = cycleCheck[from];
            } else if (cycleCheck[from] == cycleCheck[to]) {
                //cycle found
                continue;
            } else {
                auto m = std::max(cycleCheck[from], cycleCheck[to]);
                for (int j = 0; j < cycleCheck.size(); ++j) {
                    if (cycleCheck[j] == m) {
                        cycleCheck[j] = std::min(cycleCheck[from], cycleCheck[to]);
                    }
                }
            }
            spanningAdj[from].push_back(to);
            spanningAdj[to].push_back(from);
            spanningTreeWeight += weight;
        }
    }
    hex.cliqueParents.resize(numCliques, -1);
    hex.cliqueChildren.resize(numCliques);
    {
        auto currClique = 0;
        std::vector<bool> visited(numCliques);
        while (true) {
            auto cliqNei = spanningAdj[currClique];
            auto visitChild = false;
            auto cParent = hex.cliqueParents[currClique];
            for (auto nei:cliqNei) {
                if (nei != cParent && !visited[nei]) {
                    visitChild = true;
                    hex.cliqueParents[nei] = currClique;
                    currClique = nei;
                    break;
                }
            }
            if (visitChild) {
                continue;
            }
            visited[currClique] = true;
            auto children = spanningAdj[currClique];
            if (cParent >= 0) {
                for (int i = 0; i < children.size(); ++i) {
                    if (children[i] == cParent) {
                        children[i] = children.back();
                        children.pop_back();
                    }
                }
            }
            hex.cliqueChildren[currClique] = children;
            hex.upPass.push_back(currClique);
            if (cParent >= 0) {
                currClique = cParent;
            } else {
                break;
            }
        }
    }
}

void listCliqueStateSpace(HexGraphIntermediate &hex, std::vector<int> clique, std::vector<std::set<int>> &states,
                          std::set<int> curState) {
    if (clique.empty())return;
    auto fixed = clique.back();
    clique.pop_back();

    // set fixed to 0
    {
        // find all children of fixed
        auto tempClique = clique;
        auto children = hex.denseParentToChild[fixed];
        auto it = tempClique.begin();
        while (it != tempClique.end()) {
            if (children.find(*it) != children.end()) {
                it = tempClique.erase(it);
            } else {
                ++it;
            }
        }
        if (tempClique.empty()) {
            if (!curState.empty())
                states.push_back(curState);
        } else {
            listCliqueStateSpace(hex, tempClique, states, curState);
        }
    }
    // set fixed to 1
    {
        auto tempClique = clique;
        auto parents = hex.denseChildToParent[fixed];
        auto exclusions = hex.denseExclusions[fixed];
        curState.insert(fixed);
        {
            auto it = tempClique.begin();
            while (it != tempClique.end()) {
                if (parents.find(*it) != parents.end()) {
                    curState.insert(*it);
                    it = tempClique.erase(it);
                } else {
                    ++it;
                }
            }
        }
        {
            auto it = tempClique.begin();
            while (it != tempClique.end()) {
                if (exclusions.find(*it) != exclusions.end()) {
                    it = tempClique.erase(it);
                } else {
                    ++it;
                }
            }
        }
        if (tempClique.empty()) {
            states.push_back(curState);
        } else {
            listCliqueStateSpace(hex, tempClique, states, curState);
        }
    }
}

void listStateSpace(HexGraphIntermediate &hex) {
    hex.states.resize(hex.cliques.size());
    for (int i = 0; i < hex.cliques.size(); ++i) {
        listCliqueStateSpace(hex, hex.cliques[i], hex.states[i]);
    }
}

void recordSumProduct(HexGraphIntermediate &hex) {
    /*
     * For each clique
     * neigh = children .. parent
     * for each neigh n
     * for each state in clique s
     * stores the set of idx of the state in nei such that
     * the intersection of clique and nei clique is entirely
     * rep in state and nei state (idx)
     * */

    hex.supProduct.resize(hex.cliques.size());
    for (int currClIdx = 0; currClIdx < hex.cliques.size(); ++currClIdx) {
        auto currClique = hex.cliques[currClIdx];
        auto neighCliques = hex.cliqueChildren[currClIdx];
        if (hex.cliqueParents[currClIdx] >= 0) {
            neighCliques.push_back(hex.cliqueParents[currClIdx]);
        }
        for (int neiClIdx = 0; neiClIdx < neighCliques.size(); ++neiClIdx) {
            auto neiClique = hex.cliques[neighCliques[neiClIdx]];
            std::set<int> intersectVarCliques;
            {
                auto c1 = currClique;
                auto c2 = neiClique;
                std::sort(c1.begin(), c1.end());
                std::sort(c2.begin(), c2.end());
                std::set_intersection(c1.begin(), c1.end(), c2.begin(), c2.end(), intersectVarCliques.begin());
            }
            for (int currClStateIdx = 0; currClStateIdx < hex.states[currClIdx].size(); ++currClStateIdx) {
                auto currClState = hex.states[currClIdx][currClStateIdx];
                std::set<int> currClStateInter;
                std::set_intersection(currClState.begin(), currClState.end(), intersectVarCliques.begin(),
                                      intersectVarCliques.end(), currClStateInter.begin());

                for (int neiClStateIdx = 0; neiClStateIdx < hex.states[neiClIdx].size(); ++neiClStateIdx) {
                    auto neiClState = hex.states[neiClIdx][neiClStateIdx];
                    std::set<int> neiClStateInter;
                    std::set_intersection(neiClState.begin(), neiClState.end(), intersectVarCliques.begin(),
                                          intersectVarCliques.end(), neiClStateInter.begin());
                    if (currClStateInter == neiClStateInter) {
                        hex.supProduct[currClIdx][std::make_pair(neiClIdx, currClStateIdx)].push_back(neiClStateIdx);
                    }
                }

            }

        }

    }

}


