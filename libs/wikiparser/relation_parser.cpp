//
// Created by nvr on 18/7/19.
//

#include <fstream>
#include <iostream>
#include "wikiparser/relation_parser.h"

std::vector<std::string_view> parse_line(std::string_view line) {
    std::vector<std::string_view> finds;
    if (line.empty())
        return finds;
    if (line.at(0) == '#')
        return finds;

    while (!line.empty()) {
        char end_char = 0;
        if (line.at(0) == '"') {
            end_char = '"';
        } else if (line.at(0) == '<') {
            end_char = '>';
        } else {
            line.remove_prefix(1);
            continue;
        }
        line.remove_prefix(1);
        size_t end_at = line.find_first_of(end_char);
        if (end_at == std::string_view::npos) {
            break;
        }
        finds.push_back(line.substr(0, end_at));
        line.remove_prefix(end_at);
    }
    return finds;
}

void parserelation(const std::string &filename,
                            const std::function<void(std::vector<std::string_view>)> &collector) {
    std::ifstream ifile;
    ifile.open(filename);
    std::string line;
    int doc_count = 0;
    while (std::getline(ifile, line)) {
        collector(parse_line(line));
        if (++doc_count % 10000 == 0) {
            std::cout << "processing. done:" << doc_count << std::endl;
        }
    }
}
