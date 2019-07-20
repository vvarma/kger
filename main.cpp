#include <iostream>
#include <memory>
#include "relation_parser.h"
#include "graph.h"

using namespace std;

void print_vec(vector<string_view> finds) {
    if (finds.empty()) {
        cout << "empty\n";
    } else {
        cout << finds.size() << '\n';
    }
}

int main() {
    cout << "Hello, World!" << std::endl;
    string filename = "/home/nvr/CLionProjects/kger/data/instance_types_en_10k.ttl";
    //std::cin >> filename;
    relation_parser rp;
    auto builder = make_shared<GraphBuilder>(make_unique<Registery>());
    auto lambda = [builder](vector<string_view> finds) -> void {
        builder->relation_collector(finds);
    };
    rp.parse(filename, lambda);
    cout << *builder;
    return 0;
}