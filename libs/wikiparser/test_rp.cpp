#include <wikiparser/relation_parser.h>
#include <iostream>

//
// Created by nvr on 24/8/19.
//
int main() {
    auto lambda = [](std::vector<std::string_view> finds) -> void {
        for (auto a :finds) {
            std::cout << a << std::endl;

        }
    };
    parse_relation("/home/nvr/CLionProjects/kger/data/instance_types_en_100k.ttl", lambda);
}

